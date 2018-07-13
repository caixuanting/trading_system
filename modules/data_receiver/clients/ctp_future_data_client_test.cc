// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "modules/data_receiver/clients/ctp_future_data_client.h"

#include <thread>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/test/unit_test.hpp>

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

using boost::gregorian::date;
using boost::gregorian::day_clock;
using event_scheduler::Event;
using google::protobuf::TextFormat;
using interface::Data;
using interface::DataPool;
using interface::Security;
using interface::SecurityPool;
using std::string;
using std::this_thread::sleep_for;
using std::thread;
using std::to_string;
using std::unique_ptr;
using util::Queue;

namespace data_receiver {

struct CtpFutureDataClientFixture {
  CtpFutureDataClientFixture() {
    date today = day_clock::local_day();
    unsigned short year = today.year();
    unsigned short month = today.month();

    year += month / 12;
    year = year % 100;
    month = (month + 1) % 12;

    security_id =
        "cu" + to_string(year) + to_string(month / 10) + to_string(month % 10);

    CtpFutureDataClientConfig config;

    TextFormat::ParseFromString(
        R"(name: "test_client")"
        R"(broker_id: "9999")"
        R"(user_id: "015236")"
        R"(password: "123456")"
        R"(log_dir: "test/ctp_future/")"
        R"(frontend_address: "tcp://180.168.146.187:10010")"
        R"(time_interval {)"
        R"(  begin_time: "09:00:00")"
        R"(  end_time: "11:30:00")"
        R"(})"
        R"(time_interval {)"
        R"(  begin_time: "13:30:00")"
        R"(  end_time: "15:00:00")"
        R"(})",
        &config);

    SecurityPool security_pool;

    data_pool.reset(new DataPool(security_pool));

    Security security;
    TextFormat::ParseFromString(
        R"(country: "cn")"
        R"(exchange: "shfe")"
        R"(security_type: CTP_FUTURE)",
        &security);
    security.set_id(security_id);
    security.set_name(security_id);

    Data* data = new Data(security);

    data_pool->InsertData(security_id, data);

    event_queue.reset(new Queue<Event*>());
    data_message_queue.reset(new Queue<DataMessage>());

    ctp_future_data_client.reset(new CtpFutureDataClient(
        config, data_pool.get(), event_queue.get(), data_message_queue.get()));
  }

  virtual ~CtpFutureDataClientFixture() {}

  unique_ptr<CtpFutureDataClient> ctp_future_data_client;
  unique_ptr<DataPool> data_pool;
  unique_ptr<Queue<Event*>> event_queue;
  unique_ptr<Queue<DataMessage>> data_message_queue;
  string security_id;
};

BOOST_FIXTURE_TEST_SUITE(CtpFutureDataClientTests, CtpFutureDataClientFixture)

BOOST_AUTO_TEST_CASE(Run) {
  ctp_future_data_client->Init();

  thread start_thread([this] { this->ctp_future_data_client->Start(); });

  sleep_for(std::chrono::seconds(1));

  ctp_future_data_client->UserLogin();

  Data* data = data_pool->GetDataById(security_id);
  BOOST_CHECK(data != nullptr);

  sleep_for(std::chrono::seconds(2));
  LOG(INFO) << data->GetLastSnapshot().DebugString();

  sleep_for(std::chrono::seconds(1));
  ctp_future_data_client->UnSubscribeData();

  sleep_for(std::chrono::seconds(1));
  ctp_future_data_client->Exit();

  start_thread.join();

  BOOST_CHECK(event_queue->Size() != 0);
  BOOST_CHECK(data_message_queue->Size() != 0);

  DataMessage data_message = data_message_queue->Next();
  LOG(INFO) << data_message.DebugString();
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace data_receiver
