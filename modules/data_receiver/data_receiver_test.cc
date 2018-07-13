// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "modules/data_receiver/data_receiver.h"

#include <thread>

#include <boost/test/unit_test.hpp>

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

using event_scheduler::Event;
using google::protobuf::TextFormat;
using interface::DataPool;
using interface::SecurityPool;
using std::unique_ptr;
using util::Queue;
using std::this_thread::sleep_for;
using std::thread;

namespace data_receiver {

struct DataReceiverFixture {
  DataReceiverFixture() {
    DataReceiverConfig data_receiver_config;

    TextFormat::ParseFromString(
        R"(ctp_future_data_client_config {)"
        R"(  name: "YIN_HE_CTP_FUTURE")"
        R"(  log_dir: "test/ctp_future/")"
        R"(  frontend_address: "tcp://180.168.146.187:10010")"
        R"(  broker_id: "9999")"
        R"(  user_id: "015236")"
        R"(  password: "123456")"
        R"(  time_interval {)"
        R"(    begin_time: "09:00:00")"
        R"(    end_time: "15:00:00")"
        R"(  })"
        R"(  time_interval {)"
        R"(    begin_time: "21:00:00")"
        R"(    end_time: "02:30:00")"
        R"(  })"
        R"(})"
        R"(data_dir: "test")",
        &data_receiver_config);

    SecurityPool security_pool;

    TextFormat::ParseFromString(
        R"(security {)"
        R"(  country: "cn")"
        R"(  exchange: "czce")"
        R"(  id: "cn.czce.SR705")"
        R"(  name: "SR705")"
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

    event_queue.reset(new Queue<Event*>());
    data_message_queue.reset(new Queue<DataMessage>());
    data_receiver.reset(new DataReceiver(data_receiver_config, data_pool.get(),
                                         event_queue.get(),
                                         data_message_queue.get()));
  }

  unique_ptr<DataPool> data_pool;
  unique_ptr<Queue<Event*>> event_queue;
  unique_ptr<Queue<DataMessage>> data_message_queue;
  unique_ptr<DataReceiver> data_receiver;
};

BOOST_FIXTURE_TEST_SUITE(DataReceiverTest, DataReceiverFixture)

BOOST_AUTO_TEST_CASE(EndToEndTest) {
  data_receiver->Init();

  // 4 data dump events + 2 user log in events + 2 unsubscribe events.
  BOOST_CHECK_EQUAL(8, event_queue->Size());

  data_receiver->Start();

  sleep_for(std::chrono::seconds(1));

  thread print_thread([this] {
    for (int i = 0; i < 3; i++) {
      LOG(INFO) << this->data_pool->GetDataById("cn.czce.SR705")
                       ->GetLastSnapshot()
                       .DebugString();
      sleep_for(std::chrono::seconds(1));
    }

  });

  sleep_for(std::chrono::seconds(4));

  data_receiver->Exit();

  print_thread.join();
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace data_receiver