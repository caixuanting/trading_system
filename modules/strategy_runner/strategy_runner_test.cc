// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "modules/strategy_runner/strategy_runner.h"

#include <boost/test/unit_test.hpp>

#include <google/protobuf/text_format.h>

namespace strategy_runner {

struct StrategyRunnerFixture {
  StrategyRunnerFixture() {}

  virtual ~StrategyRunnerFixture() {}

  unique_ptr<StrategyRunner> strategy_runner;
};

BOOST_FIXTURE_TEST_SUITE(StrategyRunnerTests, StrategyRunnerFixture)

BOOST_AUTO_TEST_CASE(EndToEndTest) {}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace strategy_runner