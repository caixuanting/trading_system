// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "http_server/http_server.h"

#include <exception>

#include <glog/logging.h>

using boost::network::http::server;
using event_scheduler::DescriptionToEvent;
using interface::DataPool;
using interface::SignalPool;
using std::exception;
using std::thread;

namespace http_server {

HttpServer::HttpServer(const HttpServerConfig& config,
                       const DescriptionToEvent& description_to_event,
                       DataPool* data_pool, SignalPool* signal_pool)
    : config_(config),
      description_to_event_(description_to_event),
      data_pool_(data_pool),
      signal_pool_(signal_pool) {}

HttpServer::~HttpServer() {}

void HttpServer::Init() {
  LOG(INFO) << "Initing the http server";

  http_server_handler_.reset(new HttpServerHandler(
      config_, description_to_event_, data_pool_, signal_pool_));
  options_.reset(new server<HttpServerHandler>::options(*http_server_handler_));
  options_->address(config_.address());
  options_->port(config_.port());
  options_->reuse_address(true);
  server_.reset(new server<HttpServerHandler>(*options_));

  LOG(INFO) << "Inited the http server";
}

void HttpServer::Start() {
  LOG(INFO) << "Starting the http server";

  server_thread_ = thread([this] { this->server_->run(); });

  LOG(INFO) << "Started the http server";
}

void HttpServer::Exit() {
  LOG(INFO) << "Exiting the http server";
  server_->stop();

  server_thread_.join();

  LOG(INFO) << "Exited the http server";
}

bool HttpServer::Running() { return http_server_handler_->Running(); }

}  // namespace http_server
