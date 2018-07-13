// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "interfaces/signal/signal_simple.h"

#include <boost/test/unit_test.hpp>

#include <google/protobuf/text_format.h>

using google::protobuf::RepeatedPtrField;
using google::protobuf::TextFormat;
using std::unique_ptr;
using std::vector;
using util::TimeInterval;

namespace interface {

struct SignalSimpleFixture {
  SignalSimpleFixture() {
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
          R"(action_day: "2016-12-31")"
          R"(exchange_time: "2016-12-31 10:00:00")"
          R"(local_time: "2016-12-31 10:00:00")"
          R"(level_1_bid_price: 10)"
          R"(is_valid: true)",
          &snapshot);

      data->InsertSnapshot(snapshot);
    }
    {
      Snapshot snapshot;

      TextFormat::ParseFromString(
          R"(exchange_id: "test_exchange")"
          R"(instrument_id: "test_instrument_id")"
          R"(action_day: "2016-12-31")"
          R"(exchange_time: "2016-12-31 10:59:59")"
          R"(local_time: "2016-12-31 10:01:00")"
          R"(level_1_bid_price: 10.5)"
          R"(is_valid: true)",
          &snapshot);

      data->InsertSnapshot(snapshot);
    }
    {
      Snapshot snapshot;

      TextFormat::ParseFromString(
          R"(exchange_id: "test_exchange")"
          R"(instrument_id: "test_instrument_id")"
          R"(action_day: "2016-12-31")"
          R"(exchange_time: "2016-12-31 10:01:00")"
          R"(local_time: "2016-12-31 10:01:00")"
          R"(level_1_bid_price: 11)"
          R"(is_valid: true)",
          &snapshot);

      data->InsertSnapshot(snapshot);
    }

    RepeatedPtrField<TimeInterval> time_intervals;

    signal_simple.reset(new SignalSimple("TestSignalSimple", time_intervals,
                                         data.get(), LEVEL_1_BID_PRICE));
  }

  virtual ~SignalSimpleFixture() {}

  unique_ptr<Data> data;
  unique_ptr<SignalSimple> signal_simple;
};

BOOST_FIXTURE_TEST_SUITE(SignalSimpleTest, SignalSimpleFixture)

BOOST_AUTO_TEST_CASE(GetLastSignalTest) {
  Signal signal = signal_simple->GetLastSignal();

  BOOST_CHECK(signal.is_valid());
  BOOST_CHECK_EQUAL(DOUBLE, signal.value_type());
  BOOST_CHECK_EQUAL(11, signal.double_value());
}

BOOST_AUTO_TEST_CASE(GetSignalWithinTest) {
  {
    Signal signal =
        signal_simple->GetLastSignalWithin("2016-12-31 10:00:01", 1);

    BOOST_CHECK_EQUAL(DOUBLE, signal.value_type());
    BOOST_CHECK(signal.is_valid());
    BOOST_CHECK_EQUAL(10, signal.double_value());
  }
  {
    Signal signal =
        signal_simple->GetLastSignalWithin("2016-12-31 10:01:01", 5);

    BOOST_CHECK_EQUAL(DOUBLE, signal.value_type());
    BOOST_CHECK(signal.is_valid());
    BOOST_CHECK_EQUAL(11, signal.double_value());
  }
}

BOOST_AUTO_TEST_SUITE_END()
}
