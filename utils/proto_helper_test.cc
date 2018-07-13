// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "utils/proto_helper.h"

#include <boost/test/unit_test.hpp>

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include "utils/proto/time_interval.pb.h"

using google::protobuf::TextFormat;

namespace util {

struct ProtoHelperFixture {};

BOOST_FIXTURE_TEST_SUITE(ProtoHelperTests, ProtoHelperFixture)

BOOST_AUTO_TEST_CASE(ProtoFromStringTest) {
  TimeInterval time_interval = ProtoFromString(
      R"(begin_time: "2017-01-01 10:00:00")"
      R"(end_time: "2017-01-01 10:30:00")");

  LOG(INFO) << ToLineString(time_interval);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace util
