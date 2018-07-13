// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "modules/signal_manager/signal_manager.h"

#include <boost/test/unit_test.hpp>

#include <google/protobuf/text_format.h>

using data_receiver::DataMessage;
using event_scheduler::Event;
using google::protobuf::TextFormat;
using interface::DataPool;
using interface::SecurityPool;
using interface::SignalConfigs;
using interface::SignalPool;
using std::unique_ptr;
using util::Queue;

namespace signal_manager {

struct SignalManagerFixture {
  SignalManagerFixture() {
    SecurityPool security_pool;

    TextFormat::ParseFromString(
        R"(security {)"
        R"(  country: "cn")"
        R"(  exchange: "czce")"
        R"(  id: "cn.czce.SR705")"
        R"(  symbol: "SR705")"
        R"(  security_type: CTP_FUTURE)"
        R"(  multiplier: 10)"
        R"(  tick: 1.0)"
        R"(  trading_interval {)"
        R"(    begin_time: "09:00:00")"
        R"(    end_time: "10:15:00")"
        R"(  })"
        R"(  trading_interval {)"
        R"(    begin_time: "10:30:00")"
        R"(    end_time: "11:30:00")"
        R"(  })"
        R"(  trading_interval {)"
        R"(    begin_time: "13:30:00")"
        R"(    end_time: "15:00:00")"
        R"(  })"
        R"(  trading_interval {)"
        R"(    begin_time: "21:00:00")"
        R"(    end_time: "23:30:00")"
        R"(  })"
        R"(})",
        &security_pool);

    data_pool.reset(new DataPool(security_pool));

    SignalConfigs signal_configs;

    TextFormat::ParseFromString(
        R"(signal_config {)"
        R"(  name: "SR705")"
        R"(  signal_type: SIMPLE)"
        R"(  data_id: "test_data_id")"
        R"(  data_type: LEVEL_1_BID_PRICE)"
        R"(  time_interval {)"
        R"(    begin_time: "09:00:00")"
        R"(    end_time: "10:00:00")"
        R"(  })"
        R"(  time_interval {)"
        R"(    begin_time: "11:00:00")"
        R"(    end_time: "12:00:00")"
        R"(  })"
        R"(})",
        &signal_configs);

    signal_pool.reset(new SignalPool(signal_configs, data_pool.get()));

    SignalManagerConfig config;

    TextFormat::ParseFromString(
        R"(database_config {)"
        R"(  host: "")"
        R"(  user: "")"
        R"(  password: "")"
        R"(  database: "")"
        R"(})"
        R"(signal_dir: "test")",
        &config);

    event_queue.reset(new Queue<Event*>());
    data_message_queue.reset(new Queue<DataMessage>());
    signal_message_queue.reset(new Queue<SignalMessage>());

    signal_manager.reset(new SignalManager(
        config, signal_pool.get(), event_queue.get(), data_message_queue.get(),
        signal_message_queue.get()));
  }

  virtual ~SignalManagerFixture() {}

  unique_ptr<DataPool> data_pool;
  unique_ptr<Queue<DataMessage>> data_message_queue;
  unique_ptr<Queue<Event*>> event_queue;
  unique_ptr<Queue<SignalMessage>> signal_message_queue;
  unique_ptr<SignalManager> signal_manager;
  unique_ptr<SignalPool> signal_pool;
};

BOOST_FIXTURE_TEST_SUITE(SignalManagerTest, SignalManagerFixture)

BOOST_AUTO_TEST_CASE(EndToEndTest) {
  signal_manager->Init();
  signal_manager->Start();

  BOOST_CHECK_EQUAL(4, event_queue->Size());

  std::this_thread::sleep_for(std::chrono::seconds(1));

  DataMessage data_message;
  TextFormat::ParseFromString(
      R"(data_id: "test_data_id")"
      R"(timestamp: "2016-10-10 10:00:00")",
      &data_message);

  data_message_queue->Insert(data_message);
  data_message_queue->Notify();

  std::this_thread::sleep_for(std::chrono::seconds(10));

  SignalMessage signal_message = signal_message_queue->Next();

  BOOST_CHECK_EQUAL("test_data_id", signal_message.data_id());
  BOOST_CHECK_EQUAL("2016-10-10 10:00:00",
                    signal_message.timestamp_from_data());

  signal_manager->Exit();
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace signal_manager