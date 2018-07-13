// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "http_server/http_server.h"

#include <boost/test/unit_test.hpp>

#include <google/protobuf/text_format.h>

#include <glog/logging.h>

#include "modules/event_scheduler/event.h"

using event_scheduler::DescriptionToEvent;
using google::protobuf::TextFormat;
using interface::Data;
using interface::DataPool;
using interface::SecurityPool;
using interface::SignalConfigs;
using interface::SignalPool;
using interface::Snapshot;
using std::string;
using std::to_string;
using std::unique_ptr;

namespace http_server {

struct HttpServerFixture {
  HttpServerFixture() {
    HttpServerConfig config;

    TextFormat::ParseFromString(
        R"(address: "0.0.0.0")"
        R"(port: "12345")"
        R"(log_dir: "test/log")",
        &config);

    SecurityPool security_pool;

    TextFormat::ParseFromString(
        R"(security {)"
        R"(  country: "cn")"
        R"(  exchange: "CFFEX")"
        R"(  id: "cn.CFFEX.IC1701")"
        R"(  name: "IC1701")"
        R"(  security_type: CTP_FUTURE)"
        R"(  multiplier: 10)"
        R"(  tick: 1.0)"
        R"(  time_interval {)"
        R"(    begin_time: "09:00:00")"
        R"(    end_time: "10:15:00")"
        R"(  })"
        R"(  time_interval {)"
        R"(    begin_time: "10:30:00")"
        R"(    end_time: "11:30:00")"
        R"(  })"
        R"(  time_interval {)"
        R"(    begin_time: "13:30:00")"
        R"(    end_time: "15:00:00")"
        R"(  })"
        R"(  time_interval {)"
        R"(    begin_time: "21:00:00")"
        R"(    end_time: "23:30:00")"
        R"(  })"
        R"(})",
        &security_pool);

    data_pool.reset(new DataPool(security_pool));

    SignalConfigs signal_configs;

    TextFormat::ParseFromString(
        R"(signal_config {)"
        R"(  name: "SimpleDataSignal")"
        R"(  signal_type: SIMPLE)"
        R"(})"
        R"(signal_config {)"
        R"(  name: "SimpleTimeSignal")"
        R"(  signal_type: SIMPLE)"
        R"(})"
        R"(signal_config {)"
        R"(  name: "MovingAverageSignalOne")"
        R"(  signal_type: MOVING_AVERAGE)"
        R"(  data_signal_name: "SimpleDataSignal")"
        R"(  time_signal_name: "SimpleTimeSignal")"
        R"(})"
        R"(signal_config {)"
        R"(  name: "MovingAverageSignalTwo")"
        R"(  signal_type: MOVING_AVERAGE)"
        R"(  data_signal_name: "SimpleDataSignal")"
        R"(  time_signal_name: "SimpleTimeSignal")"
        R"(})",
        &signal_configs);

    signal_pool.reset(new SignalPool(signal_configs, data_pool.get()));

    Data* data = data_pool->GetDataById("cn.CFFEX.IC1701");
    data->ReadFromFile("test/data/data");

    http_server.reset(new HttpServer(config, description_to_event,
                                     data_pool.get(), signal_pool.get()));
  }

  virtual ~HttpServerFixture() {}

  DescriptionToEvent description_to_event;
  unique_ptr<DataPool> data_pool;
  unique_ptr<HttpServer> http_server;
  unique_ptr<SignalPool> signal_pool;
};

BOOST_FIXTURE_TEST_SUITE(HttpServerTest, HttpServerFixture)

BOOST_AUTO_TEST_CASE(AcceptTest) {
  http_server->Init();

  http_server->Start();

  while (http_server->Running()) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  http_server->Exit();
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace Util
