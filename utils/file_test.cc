// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include <thread>

#include <boost/test/unit_test.hpp>

#include "utils/file.h"

using std::string;
using std::vector;

namespace util {

struct FileFixture {
  FileFixture() {}

  ~FileFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(FileTest, FileFixture)

BOOST_AUTO_TEST_CASE(ReadFileTest) {
  BOOST_CHECK_EQUAL("test", ReadFile("test/one_line_file"));
}

BOOST_AUTO_TEST_CASE(ReadFileLinesTest) {
  vector<string> lines = ReadFileLines("test/two_line_file");

  BOOST_CHECK_EQUAL(2, lines.size());
  BOOST_CHECK_EQUAL("line 1", lines[0]);
  BOOST_CHECK_EQUAL("line 2", lines[1]);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace util
