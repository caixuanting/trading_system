// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "interfaces/account/account.h"

#include <boost/test/unit_test.hpp>

#include <google/protobuf/text_format.h>

using google::protobuf::TextFormat;

namespace interface {

struct AccountFixture {
  AccountFixture() {}

  virtual ~AccountFixture() {}

  std::unique_ptr<Account> account;
};

BOOST_FIXTURE_TEST_SUITE(AccountTests, AccountFixture)

BOOST_AUTO_TEST_CASE() {}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace interface
