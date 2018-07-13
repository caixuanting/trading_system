// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "modules/account_keeper/account_keeper.h"

#include <thread>

#include <boost/test/unit_test.hpp>

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

using google::protobuf::TextFormat;
using interface::Account;
using interface::AccountOperator;
using interface::AccountPool;
using interface::Order;
using std::unique_ptr;
using util::Queue;

namespace account_keeper {

struct AccountKeeperFixture {
  AccountKeeperFixture() {
    AccountKeeperConfig config;

    TextFormat::ParseFromString(
        R"(database_config {)"
        R"(  host: "localhost")"
        R"(  user: "root")"
        R"(  password: "cai1984")"
        R"(  database: "test")"
        R"(})",
        &config);

    account_pool.reset(new AccountPool());
    account_message_queue.reset(new Queue<AccountMessage>());

    account_keeper.reset(new AccountKeeper(config, account_pool.get(),
                                           account_message_queue.get()));
  }

  virtual ~AccountKeeperFixture() {}

  unique_ptr<AccountPool> account_pool;
  unique_ptr<Queue<AccountMessage>> account_message_queue;
  unique_ptr<AccountKeeper> account_keeper;
};

BOOST_FIXTURE_TEST_SUITE(AccountKeeperTests, AccountKeeperFixture)

BOOST_AUTO_TEST_CASE(EndToEndTest) {
  account_keeper->Init();
  account_keeper->Start();

  std::this_thread::sleep_for(std::chrono::seconds(1));

  AccountOperator* account_operator =
      account_pool->GetAccountOperatorByAccountId("test_account");

  BOOST_CHECK(account_operator != nullptr);

  {
    Account account = account_operator->GetAccount();

    BOOST_CHECK_EQUAL("test_account", account.id());
    BOOST_CHECK_EQUAL(100, account.cash());
  }

  Order order;
  TextFormat::ParseFromString(R"(id: 1)", &order);

  account_operator->InsertOrder(order);

  {
    AccountMessage account_message;
    TextFormat::ParseFromString(
        R"(account_id: "test_account")", &account_message);
    account_message_queue->Insert(account_message);
    account_message_queue->Notify();
  }

  {
    Account account = account_operator->GetAccount();

    LOG(INFO) << account.DebugString();
  }

  {
    AccountMessage account_message;
    TextFormat::ParseFromString(
        R"(account_id: "test_account")", &account_message);
    account_message_queue->Insert(account_message);
    account_message_queue->Notify();
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));

  account_operator->UpdateCancelOrder(1);

  {
    AccountMessage account_message;
    TextFormat::ParseFromString(
        R"(account_id: "test_account")", &account_message);
    account_message_queue->Insert(account_message);
    account_message_queue->Notify();
  }

  account_keeper->Exit();
}

BOOST_AUTO_TEST_CASE(UpdateTest) {
  account_keeper->Init();
  account_keeper->Start();

  std::this_thread::sleep_for(std::chrono::seconds(1));

  for (int i = 0; i < 10; i++) {
    AccountMessage account_message;
    TextFormat::ParseFromString(
        R"(account_id: "test_account")", &account_message);
    account_message_queue->Insert(account_message);
    account_message_queue->Notify();

    LOG(INFO) << "Running iteration " << i;

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  account_keeper->Exit();
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace account_keeper