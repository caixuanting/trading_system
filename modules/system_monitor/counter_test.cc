// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include <boost/test/unit_test.hpp>

#include "modules/system_monitor/counter.h"

namespace system_monitor {

struct CounterFixture {
  CounterFixture() {}

  virtual ~CounterFixture() {}

  Counter counter;
};

BOOST_FIXTURE_TEST_SUITE(CounterTest, CounterFixture)

BOOST_AUTO_TEST_CASE(Increment) {
  BOOST_CHECK_EQUAL(0, counter.GetCurrentTotal());
  BOOST_CHECK_EQUAL(0, counter.GetPreviousTotal());
  BOOST_CHECK_EQUAL(0, counter.GetDiff());

  counter.Increment(5);
  BOOST_CHECK_EQUAL(5, counter.GetCurrentTotal());
  BOOST_CHECK_EQUAL(0, counter.GetPreviousTotal());
  BOOST_CHECK_EQUAL(5, counter.GetDiff());

  counter.UpdatePreviousTotal();
  BOOST_CHECK_EQUAL(5, counter.GetCurrentTotal());
  BOOST_CHECK_EQUAL(5, counter.GetPreviousTotal());
  BOOST_CHECK_EQUAL(0, counter.GetDiff());

  counter.Increment();
  BOOST_CHECK_EQUAL(6, counter.GetCurrentTotal());
  BOOST_CHECK_EQUAL(5, counter.GetPreviousTotal());
  BOOST_CHECK_EQUAL(1, counter.GetDiff());

  counter.SetCurrentTotal(100);
  BOOST_CHECK_EQUAL(100, counter.GetCurrentTotal());
  BOOST_CHECK_EQUAL(5, counter.GetPreviousTotal());
  BOOST_CHECK_EQUAL(95, counter.GetDiff());
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace system_monitor
