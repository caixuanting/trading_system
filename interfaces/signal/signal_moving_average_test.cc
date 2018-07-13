// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include <boost/test/unit_test.hpp>

#include "interfaces/signal/signal_moving_average.h"

#include <google/protobuf/text_format.h>

#include "interfaces/signal/signal_simple.h"

using google::protobuf::RepeatedPtrField;
using google::protobuf::TextFormat;
using std::unique_ptr;
using std::vector;

namespace interface {

struct SignalMovingAverageFixture {
  SignalMovingAverageFixture() {
    Security security;
    TextFormat::ParseFromString(
        R"(country: "test_country")"
        R"(exchange: "test_exchange")"
        R"(id: "test_id")",
        &security);

    data.reset(new Data(security));
    {
      Snapshot snapshot;

      TextFormat::ParseFromString(
          R"(exchange_id: "test_exchange")"
          R"(instrument_id: "test_instrument_id")"
          R"(action_day: "2016-10-10")"
          R"(exchange_time: "2016-10-10 10:00:00")"
          R"(local_time: "2016-10-10 10:00:00")"
          R"(level_1_bid_price: 1)"
          R"(is_valid: true)",
          &snapshot);
      data->InsertSnapshot(snapshot);
    }
    {
      Snapshot snapshot;

      TextFormat::ParseFromString(
          R"(exchange_id: "test_exchange")"
          R"(instrument_id: "test_instrument_id")"
          R"(action_day: "2016-10-10")"
          R"(exchange_time: "2016-10-10 10:00:10")"
          R"(local_time: "2016-10-10 10:00:10")"
          R"(level_1_bid_price: 2)"
          R"(is_valid: true)",
          &snapshot);
      data->InsertSnapshot(snapshot);
    }
    {
      Snapshot snapshot;

      TextFormat::ParseFromString(
          R"(exchange_id: "test_exchange")"
          R"(instrument_id: "test_instrument_id")"
          R"(action_day: "2016-10-10")"
          R"(exchange_time: "2016-10-10 10:00:30")"
          R"(local_time: "2016-10-10 10:00:30")"
          R"(level_1_bid_price: 4)"
          R"(is_valid: true)",
          &snapshot);
      data->InsertSnapshot(snapshot);
    }
    {
      Snapshot snapshot;

      TextFormat::ParseFromString(
          R"(exchange_id: "test_exchange")"
          R"(instrument_id: "test_instrument_id")"
          R"(action_day: "2016-10-10")"
          R"(exchange_time: "2016-10-10 10:00:40")"
          R"(local_time: "2016-10-10 10:00:40")"
          R"(level_1_bid_price: 5)"
          R"(is_valid: true)",
          &snapshot);
      data->InsertSnapshot(snapshot);
    }
    {
      Snapshot snapshot;

      TextFormat::ParseFromString(
          R"(exchange_id: "test_exchange")"
          R"(instrument_id: "test_instrument_id")"
          R"(action_day: "2016-10-10")"
          R"(exchange_time: "2016-10-10 10:00:50")"
          R"(local_time: "2016-10-10 10:00:50")"
          R"(level_1_bid_price: 6)"
          R"(is_valid: true)",
          &snapshot);
      data->InsertSnapshot(snapshot);
    }
    {
      Snapshot snapshot;

      TextFormat::ParseFromString(
          R"(exchange_id: "test_exchange")"
          R"(instrument_id: "test_instrument_id")"
          R"(action_day: "2016-10-10")"
          R"(exchange_time: "2016-10-10 10:01:00")"
          R"(local_time: "2016-10-10 10:01:00")"
          R"(level_1_bid_price: 7)"
          R"(is_valid: true)",
          &snapshot);

      data->InsertSnapshot(snapshot);
    }

    RepeatedPtrField<TimeInterval> time_intervals;

    time_signal.reset(new SignalSimple("TestTimeSignal", time_intervals,
                                       data.get(), TIMESTAMP));
    data_signal.reset(new SignalSimple("TestDataSignal", time_intervals,
                                       data.get(), LEVEL_1_BID_PRICE));
    signal_moving_average.reset(new SignalMovingAverage(
        "TestSignalMovingAverage", 2, 10, time_intervals, time_signal.get(),
        data_signal.get()));
  }

  virtual ~SignalMovingAverageFixture() {}

  unique_ptr<Data> data;
  unique_ptr<SignalSimple> time_signal;
  unique_ptr<SignalSimple> data_signal;
  unique_ptr<SignalMovingAverage> signal_moving_average;
};

BOOST_FIXTURE_TEST_SUITE(SignalMovingAverageTest, SignalMovingAverageFixture)

BOOST_AUTO_TEST_CASE(RunTest) {
  signal_moving_average->LookUp(
      "localhost", "root", "cai1984", "test",
      "SELECT * FROM signals WHERE name = 'TestSignalMovingAverage'",
      "2016-10-10 10:00:00");

  {
    Signal signal = signal_moving_average->GetLatestSignal();

    BOOST_CHECK(signal.is_valid());
    BOOST_CHECK_EQUAL("2016-10-10 10:00:50.000000", signal.timestamp());
    BOOST_CHECK_EQUAL(5.5, signal.double_value());
    BOOST_CHECK_EQUAL(2, signal.base_signal_size());
  }

  {
    Signal signal =
        signal_moving_average->GetSignalWithin("2016-10-10 10:00:30", 10);

    BOOST_CHECK(signal.is_valid());
    BOOST_CHECK_EQUAL("2016-10-10 10:00:20.000000", signal.timestamp());
    BOOST_CHECK_EQUAL(1.5, signal.double_value());
    BOOST_CHECK_EQUAL(2, signal.base_signal_size());
  }
}

BOOST_AUTO_TEST_CASE(GetDependentSignalsTest) {
  vector<SignalBase*> dependent_signals =
      signal_moving_average->DependentSignals();

  BOOST_CHECK_EQUAL(1, dependent_signals.size());
  BOOST_CHECK_EQUAL("TestDataSignal", dependent_signals[0]->Name());
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace interface
