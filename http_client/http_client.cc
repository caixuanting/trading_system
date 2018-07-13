// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

#include <glog/logging.h>

#include "http_client/http_client.h"

namespace http_client {

HttpClient::HttpClient() : initialized_(false) {}

HttpClient::~HttpClient() {}

void HttpClient::ConnectSocket() {
  if ((socketfd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    LOG(ERROR) << "Socket creation error";
    
    return;
  }

  sockaddr_in server_address;

  memset(&server_address, '0', sizeof(server_address));

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(global_flag.http_client_port);

  if (inet_pton(AF_INET, global_flag.http_client_address.c_str(),
                &server_address.sin_addr) <= 0) {
    LOG(ERROR) << "Address error";
    
    return;
  }

  if (connect(socketfd_, (struct sockaddr*)&server_address,
              sizeof(server_address)) < 0) {
    LOG(ERROR) << "Socket connection error";
    
    return;
  }

  initialized_ = true;
}

void HttpClient::Send(const string& content) {
  if (initialized_) {
    int32_t result = write(socketfd_, content.c_str(), content.size());

    if (result < 0) {
      LOG(ERROR) << "Send message <" << content << "> failed";
    } else {
    	LOG(INFO) << "Send message <" << content << "> succeeded";
    }
  }
}

void HttpClient::DisconnectSocket() {
  initialized_ = false;

  if (socketfd_ != -1) {
    close(socketfd_);
    socketfd_ = -1;
  }
}

}  // namespace http_client
