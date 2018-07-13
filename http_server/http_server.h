// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef HTTP_SERVER_HTTP_SERVER_H_
#define HTTP_SERVER_HTTP_SERVER_H_

#include <thread>

#include <boost/network/protocol/http/server.hpp>

#include "http_server/http_server_handler.h"

namespace http_server {

typedef boost::network::http::server<HttpServerHandler>::options Options;

class HttpServer {
 public:
  HttpServer(const HttpServerConfig& config,
             const event_scheduler::DescriptionToEvent& description_to_event,
             interface::DataPool* data_pool,
             interface::SignalPool* signal_pool);
  virtual ~HttpServer();

  void Init();
  void Start();
  void Exit();

  bool Running();

 private:
  HttpServerConfig config_;
  const event_scheduler::DescriptionToEvent& description_to_event_;
  interface::DataPool* data_pool_;      // Not owned.
  interface::SignalPool* signal_pool_;  // Not owned.
  std::thread server_thread_;
  std::unique_ptr<HttpServerHandler> http_server_handler_;
  std::unique_ptr<Options> options_;
  std::unique_ptr<boost::network::http::server<HttpServerHandler>> server_;
};

}  // namespace http_server

#endif  // HTTP_SERVER_HTTP_SERVER_H_
