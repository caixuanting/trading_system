// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "utils/date_time.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/test/unit_test.hpp>

using boost::gregorian::date;
using boost::posix_time::ptime;
using boost::posix_time::time_from_string;

namespace util {

struct DateTimeFixture {
  DateTimeFixture() {}

  virtual ~DateTimeFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(DateTimeTest, DateTimeFixture)

BOOST_AUTO_TEST_CASE(DateTimeInStringTest) {
  ptime time = time_from_string("2016-10-23 10:00:00");

  BOOST_CHECK_EQUAL("2016-10-23 10:00:00.000000", ToString(time));
}

BOOST_AUTO_TEST_CASE(SameMinuteTest) {
  BOOST_CHECK(
      TheSameMinute("2016-01-01 10:00:00", "2016-01-01 10:00:59.999999"));

  BOOST_CHECK(
      !TheSameMinute("2016-01-01 10:00:00", "2016-01-01 10:01:00.000001"));
}

BOOST_AUTO_TEST_CASE(SameDayTest) {
  BOOST_CHECK(TheSameDay("2016-01-01 10:00:00", "2016-01-01 11:00:00"));
  BOOST_CHECK(!TheSameDay("2016-01-02 10:00:00", "2016-01-01 11:00:00"));
}

BOOST_AUTO_TEST_CASE(MinusTime) {
  ptime date_time = time_from_string("2016-10-23 10:00:00");

  BOOST_CHECK_EQUAL("2016-10-23 09:59:00.000000",
                    ToString(Minus("2016-10-23 10:00:00", 60)));
  BOOST_CHECK_EQUAL("2016-10-23 09:59:00.000000",
                    ToString(Minus(date_time, 60)));
}

BOOST_AUTO_TEST_CASE(WithinTimeIntervalTest) {
  TimeInterval time_interval;

  time_interval.set_begin_time("2016-10-10 10:00:00");
  time_interval.set_end_time("2016-10-10 10:01:00");

  BOOST_CHECK(!WithinTimeInterval("2016-10-10 09:59:59", time_interval));
  BOOST_CHECK(!WithinTimeInterval("2016-10-10 10:01:01", time_interval));
  BOOST_CHECK(WithinTimeInterval("2016-10-10 10:00:30", time_interval));
}

BOOST_AUTO_TEST_CASE(WithinTest) {
  BOOST_CHECK(Within("2016-10-10 10:00:00", "2016-10-10 10:00:05", 5));
  BOOST_CHECK(!Within("2016-10-10 10:00:00", "2016-10-10 10:00:00", 5));
  BOOST_CHECK(!Within("2016-10-10 10:00:00", "2016-10-10 10:00:05", 4));
}

BOOST_AUTO_TEST_CASE(LessOrEqualTest) {
  BOOST_CHECK(LessOrEqual("2016-10-10 10:00:00", "2016-10-10 10:00:01"));
  BOOST_CHECK(LessOrEqual("2016-10-10 10:00:00", "2016-10-10 10:00:00"));
  BOOST_CHECK(!LessOrEqual("2016-10-10 10:00:01", "2016-10-10 10:00:00"));
}

BOOST_AUTO_TEST_CASE(CreateTimeIntervalTest) {
  date d(2017, 1, 1);

  {
    TimeInterval time_interval = CreateTimeInterval("10:00:00", "10:30:00", d);
    BOOST_CHECK_EQUAL("2017-01-01 10:00:00", time_interval.begin_time());
    BOOST_CHECK_EQUAL("2017-01-01 10:30:00", time_interval.end_time());
  }

  {
    TimeInterval time_interval = CreateTimeInterval("10:30:00", "10:00:00", d);
    BOOST_CHECK_EQUAL("2017-01-01 10:30:00", time_interval.begin_time());
    BOOST_CHECK_EQUAL("2017-01-02 10:00:00", time_interval.end_time());
  }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace util
