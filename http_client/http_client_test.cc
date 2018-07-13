// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include <boost/test/unit_test.hpp>

#include "http_client/http_client.h"

using std::unique_ptr;

namespace http_client {

struct HttpClientFixture {
  HttpClientFixture() {
    http_client.reset(new HttpClient());
  }
  virtual ~HttpClientFixture() {}

  unique_ptr<HttpClient> http_client;
};

BOOST_FIXTURE_TEST_SUITE(HttpClientTest, HttpClientFixture)

BOOST_AUTO_TEST_CASE(Send) {
  http_client->ConnectSocket();

	http_client->Send("test");
	
  http_client->DisconnectSocket();
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace http_client
