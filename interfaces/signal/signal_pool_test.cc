// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "interfaces/signal/signal_pool.h"

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

using google::protobuf::TextFormat;
using std::unique_ptr;

namespace interface {

struct SignalPoolFixture {
  SignalPoolFixture() {
    SecurityPool security_pool;
    TextFormat::ParseFromString(
        R"(security {)"
        R"(  country: "test_country")"
        R"(  exchange: "test_exchange")"
        R"(  id : "test_id")"
        R"(})",
        &security_pool);

    data_pool.reset(new DataPool(security_pool));

    SignalConfigs signal_configs;

    TextFormat::ParseFromString(
        R"(signal_config {)"
        R"(  name: "SimpleDataSignal")"
        R"(  signal_type: SIMPLE)"
        R"(  data_id: "test_id")"
        R"(  time_interval {)"
        R"(    begin_time: "10:00:00")"
        R"(    end_time: "10:30:00")"
        R"(  })"
        R"(})"
        R"(signal_config {)"
        R"(  name: "SimpleTimeSignal")"
        R"(  signal_type: SIMPLE)"
        R"(  data_id: "test_id")"
        R"(  time_interval {)"
        R"(    begin_time: "10:00:00")"
        R"(    end_time: "10:30:00")"
        R"(  })"
        R"(})"
        R"(signal_config {)"
        R"(  name: "MovingAverageSignalOne")"
        R"(  signal_type: MOVING_AVERAGE)"
        R"(  data_signal_name: "SimpleDataSignal")"
        R"(  time_signal_name: "SimpleTimeSignal")"
        R"(  time_interval {)"
        R"(    begin_time: "10:00:00")"
        R"(    end_time: "10:30:00")"
        R"(  })"
        R"(})"
        R"(signal_config {)"
        R"(  name: "MovingAverageSignalTwo")"
        R"(  signal_type: MOVING_AVERAGE)"
        R"(  data_signal_name: "SimpleDataSignal")"
        R"(  time_signal_name: "SimpleTimeSignal")"
        R"(  time_interval {)"
        R"(    begin_time: "10:00:00")"
        R"(    end_time: "10:30:00")"
        R"(  })"
        R"(})",
        &signal_configs);

    signal_pool.reset(new SignalPool(signal_configs, data_pool.get()));
  }

  virtual ~SignalPoolFixture() {}

  unique_ptr<DataPool> data_pool;
  unique_ptr<SignalPool> signal_pool;
};

BOOST_FIXTURE_TEST_SUITE(SignalPoolTest, SignalPoolFixture)

BOOST_AUTO_TEST_CASE(GetSignalByNameTest) {
  {
    SignalBase* signal = signal_pool->GetSignalByName("SimpleTimeSignal");
    BOOST_CHECK(signal != nullptr);
  }

  {
    SignalBase* signal = signal_pool->GetSignalByName("SimpleDataSignal");
    BOOST_CHECK(signal != nullptr);
  }

  {
    SignalBase* signal = signal_pool->GetSignalByName("MovingAverageSignalTwo");
    BOOST_CHECK(signal != nullptr);
  }

  {
    SignalBase* signal = signal_pool->GetSignalByName("MovingAverageSignalOne");
    BOOST_CHECK(signal != nullptr);
  }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace interface
