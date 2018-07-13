// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "interfaces/signal/signal_periodic.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/test/unit_test.hpp>

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include "interfaces/signal/signal_simple.h"
#include "utils/date_time.h"

using boost::posix_time::ptime;
using boost::posix_time::seconds;
using boost::posix_time::time_from_string;
using google::protobuf::RepeatedPtrField;
using google::protobuf::TextFormat;
using std::string;
using std::unique_ptr;
using std::vector;
using util::ToString;

namespace interface {

class SignalPeriodicFake : public SignalPeriodic {
 public:
  SignalPeriodicFake(const string& name, const int32_t num_periods,
                     const int32_t period_in_seconds,
                     const RepeatedPtrField<TimeInterval>& time_intervals,
                     SignalBase* time_signal)
      : SignalPeriodic(name, num_periods, period_in_seconds, time_intervals,
                       time_signal) {}
  virtual ~SignalPeriodicFake() {}

  void ComputeNextSignal(const ptime& next_timestamp) override {
    Signal signal;

    signal.set_is_valid(true);
    signal.set_timestamp(ToString(next_timestamp));
    signal.set_value_type(INT32);
    signal.set_int32_value(signals_.size());
    signals_.push_back(signal);
  }
};

struct SignalPeriodicFixture {
  SignalPeriodicFixture() {
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
          R"(action_day: "2016-10-10")"
          R"(exchange_time: "2016-10-10 10:01:00")"
          R"(local_time: "2016-10-10 10:01:00")"
          R"(level_1_bid_price: 11)"
          R"(is_valid: true)",
          &snapshot);

      data->InsertSnapshot(snapshot);
    }

    RepeatedPtrField<TimeInterval> time_intervals;

    time_signal.reset(new SignalSimple("TestTimeSignal", time_intervals,
                                       data.get(), TIMESTAMP));
    signal_periodic.reset(new SignalPeriodicFake(
        "TestSignalPeriodic", 3, 10, time_intervals, time_signal.get()));
  }

  virtual ~SignalPeriodicFixture() {}

  unique_ptr<Data> data;
  unique_ptr<SignalSimple> time_signal;
  unique_ptr<SignalPeriodicFake> signal_periodic;
};

BOOST_FIXTURE_TEST_SUITE(SignalPeriodicTest, SignalPeriodicFixture)

BOOST_AUTO_TEST_CASE(LookUpSomeSignalTest) {
  signal_periodic->LookUp(
      "localhost", "root", "cai1984", "test",
      "SELECT * FROM signals WHERE name = 'TestSignalPeriodic'",
      "2016-10-10 10:00:00");

  Signal signal = signal_periodic->GetLatestSignal();

  BOOST_CHECK_EQUAL(INT32, signal.value_type());
  BOOST_CHECK_EQUAL(7, signal.int32_value());
  BOOST_CHECK_EQUAL("2016-10-10 10:00:50.000000", signal.timestamp());
}

BOOST_AUTO_TEST_CASE(LookUpNoSignalTest) {
  signal_periodic->LookUp(
      "localhost", "root", "cai1984", "test",
      "SELECT * FROM signals WHERE name = 'NotExistSignalPeriodic'",
      "2016-10-10 10:00:00");

  Signal signal = signal_periodic->GetLatestSignal();

  BOOST_CHECK_EQUAL(INT32, signal.value_type());
  BOOST_CHECK_EQUAL(6, signal.int32_value());
  BOOST_CHECK_EQUAL("2016-10-10 10:00:50.000000", signal.timestamp());
}

BOOST_AUTO_TEST_CASE(GetSignalWithinTest) {
  signal_periodic->LookUp(
      "localhost", "root", "cai1984", "test",
      "SELECT * FROM signals WHERE name = 'TestSignalPeriodic'",
      "2016-10-10 10:00:00");

  Signal signal = signal_periodic->GetSignalWithin("2016-10-10 10:01:00", 10);

  BOOST_CHECK(signal.is_valid());
  BOOST_CHECK_EQUAL(INT32, signal.value_type());
  BOOST_CHECK_EQUAL(7, signal.int32_value());
}

BOOST_AUTO_TEST_CASE(DumpTest) {
  signal_periodic->LookUp(
      "localhost", "root", "cai1984", "test",
      "SELECT * FROM signals WHERE name = 'TestSignalPeriodic'",
      "2016-10-10 10:00:00");

  TimeInterval time_interval;

  time_interval.set_begin_time("2016-10-10 09:00:00");
  time_interval.set_end_time("2016-10-10 10:01:00");

  signal_periodic->Dump("localhost", "root", "cai1984", "test", "test", "",
                        time_interval);

  Signal signal = signal_periodic->GetLatestSignal();

  BOOST_CHECK_EQUAL(0, signal.int32_value());
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace interface
