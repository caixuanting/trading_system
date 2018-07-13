// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef HTTP_CLIENT_HTTP_CLIENT_H_
#define HTTP_CLIENT_HTTP_CLIENT_H_

#include <string>

namespace http_client {

class HttpClient {
 public:
  HttpClient();
  virtual ~HttpClient();

  void ConnectSocket();
  void Send(const std::string& content);
  void DisconnectSocket();

 private:
  bool initialized_;
  int32_t socketfd_;
};

}  // namespace http_client

#endif  // HTTP_CLIENT_HTTP_CLIENT_H_
