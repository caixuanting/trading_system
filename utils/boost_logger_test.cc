// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "utils/boost_logger.h"

#include <boost/test/unit_test.hpp>

namespace util {

struct BoostLoggerFixture {
  BoostLoggerFixture() { InitBoostLogger("/tmp/boost_logger_test"); }
  virtual ~BoostLoggerFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(BoostLoggerTest, BoostLoggerFixture)

BOOST_AUTO_TEST_CASE(LogInfo) { LOG(INFO) << "some info message"; }

BOOST_AUTO_TEST_CASE(LogWarning) { LOG(WARNING) << "some warning message"; }

BOOST_AUTO_TEST_CASE(LogError) { LOG(ERROR) << "some error message"; }

BOOST_AUTO_TEST_CASE(LogFatal) { LOG(FATAL) << "some fatal message"; }

BOOST_AUTO_TEST_SUITE_END()

}  // namespace util
