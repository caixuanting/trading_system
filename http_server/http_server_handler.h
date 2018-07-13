// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef HTTP_SERVER_HTTP_SERVER_HANDLER_H_
#define HTTP_SERVER_HTTP_SERVER_HANDLER_H_

#include <functional>
#include <string>
#include <unordered_map>

#include <boost/network/protocol/http/server.hpp>

#include "http_server/proto/http_server_config.pb.h"
#include "interfaces/data/data_pool.h"
#include "interfaces/signal/signal_pool.h"
#include "modules/event_scheduler/event.h"

namespace http_server {

class HttpServerHandler;

typedef std::unordered_map<std::string, std::string> KeyToValue;

typedef std::unordered_map<
    std::string,
    std::function<void(
        const KeyToValue& key_to_value,
        boost::network::http::server<HttpServerHandler>::response& response)>>
    PathToHandler;

typedef boost::network::http::server<HttpServerHandler>::request Request;

typedef boost::network::http::server<HttpServerHandler>::response Response;

class HttpServerHandler {
 public:
  HttpServerHandler(
      const HttpServerConfig& config,
      const event_scheduler::DescriptionToEvent& description_to_event,
      interface::DataPool* data_pool, interface::SignalPool* signal_pool);
  virtual ~HttpServerHandler();

  void operator()(const Request& request, Response& response);

  // Required by the library.
  void log(
      const boost::network::http::server<HttpServerHandler>::string_type& info);

  bool Running();

 private:
  bool Accept(const std::string& path);
  KeyToValue ParseParameters(const std::string& query);
  std::vector<std::string> FindLogFileNames(const std::string& file_type);

  void HandleShutDownRequest(const KeyToValue& key_to_value,
                             Response& response);

  void HandleLogRequest(const KeyToValue& key_to_value, Response& response);

  void HandleSecurityRequest(const KeyToValue& key_to_value,
                             Response& response);

  std::string GetSecurityList();
  std::string GetData(const std::string& id);

  void HandleSignalRequest(const KeyToValue& key_to_value, Response& response);

  void HandleEventRequest(const KeyToValue& key_to_value, Response& response);

  HttpServerConfig config_;
  PathToHandler path_to_handler_;
  bool running_ = true;
  const event_scheduler::DescriptionToEvent& description_to_event_;
  interface::DataPool* data_pool_;      // Not owned.
  interface::SignalPool* signal_pool_;  // Not owned.
};

}  // namespace http_server

#endif  // HTTP_SERVER_HTTP_SERVER_HANDLER_H_
