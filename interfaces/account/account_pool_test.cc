// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "interfaces/account/account_pool.h"

#include <boost/test/unit_test.hpp>

#include <google/protobuf/text_format.h>

using google::protobuf::TextFormat;

namespace interface {

struct AccountPoolFixture {
  AccountPoolFixture() {
    account_pool.reset(new AccountPool());

    AccountOperator* account_operator = new AccountOperator(
        R"(id: "test_account")"
        R"(security_id_to_position_pool: {)"
        R"(  key: "test_security_id")"
        R"(  value: {)"
        R"(    position {)"
        R"(      security_id: "test_security_id")"
        R"(      quantity: 100)"
        R"(      price: 9.77)"
        R"(      timestamp: "2016-01-01 10:00:00")"
        R"(      position_type: LONG)"
        R"(    })"
        R"(    position {)"
        R"(      security_id: "test_security_id")"
        R"(      quantity: 100)"
        R"(      price: 9.77)"
        R"(      timestamp: "2016-01-01 10:01:00")"
        R"(      position_type: LONG)"
        R"(    })"
        R"(  })"
        R"(})"
        R"(order_id_to_order {)"
        R"(  key: 1)"
        R"(  value: {)"
        R"(    id: 1)"
        R"(    strategy_name: "test_strategy")"
        R"(    create_timestamp: "2017-01-01 10:00:00")"
        R"(    quantity: 100)"
        R"(    price: 9.78)"
        R"(  })"
        R"(})"
        R"(order_id_to_executed_orders: {)"
        R"(  key: 2)"
        R"(  value: {)"
        R"(    order: {)"
        R"(      id: 2)"
        R"(      strategy_name: "TestStrategy")"
        R"(      create_timestamp: "2017-01-01 10:00:00")"
        R"(      execute_timestamp: "2017-01-01 10:00:01")"
        R"(      quantity: 100)"
        R"(      price: 9.78)"
        R"(    })"
        R"(  })"
        R"(})");

    account_pool->Insert("test_id", account_operator);
  }

  virtual ~AccountPoolFixture() {}

  std::unique_ptr<AccountPool> account_pool;
};

BOOST_FIXTURE_TEST_SUITE(AccountPoolTests, AccountPoolFixture)

BOOST_AUTO_TEST_CASE(IterateAccount) {
  BOOST_CHECK_EQUAL(1, account_pool->Size());
}

BOOST_AUTO_TEST_CASE(GetAccountOperatorByAccountId) {
  {
    AccountOperator* account_operator =
        account_pool->GetAccountOperatorByAccountId("non-exist-id");
    BOOST_CHECK(account_operator == nullptr);
  }

  {
    AccountOperator* account_operator =
        account_pool->GetAccountOperatorByAccountId("test_id");
    BOOST_CHECK(account_operator != nullptr);
  }
}

BOOST_AUTO_TEST_CASE(GetAccountOperatorByOrderId) {
  {
    AccountOperator* account_operator =
        account_pool->GetAccountOperatorByOrderId(0);
    BOOST_CHECK(account_operator == nullptr);
  }

  {
    AccountOperator* account_operator =
        account_pool->GetAccountOperatorByOrderId(1);
    BOOST_CHECK(account_operator != nullptr);
  }

  {
    AccountOperator* account_operator =
        account_pool->GetAccountOperatorByOrderId(2);
    BOOST_CHECK(account_operator != nullptr);
  }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace interface
