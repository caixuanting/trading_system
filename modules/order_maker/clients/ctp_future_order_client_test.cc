// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "modules/order_maker/clients/ctp_future_order_client.h"

#include <boost/test/unit_test.hpp>

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

using event_scheduler::Event;
using google::protobuf::TextFormat;
using interface::AccountOperator;
using interface::AccountPool;
using interface::BUY;
using interface::CLOSE_TODAY;
using interface::OPEN;
using interface::Order;
using interface::SELL;
using std::string;
using std::unique_ptr;
using strategy_runner::StrategyMessage;
using util::Queue;

namespace order_maker {

struct CtpFutureOrderClientFixture {
  CtpFutureOrderClientFixture() {
    CtpFutureOrderClientConfig config;

    TextFormat::ParseFromString(
        R"(name: "test_cpt_future_order_client")"
        R"(broker_id: "9999")"
        R"(user_id: "015236")"
        R"(password: "123456")"
        R"(frontend_address: "tcp://180.168.146.187:10000")"
        R"(log_dir: "test/ctp_future/")"
        R"(time_interval {)"
        R"(  begin_time: "09:00:00")"
        R"(  end_time: "15:00:00")"
        R"(})"
        R"(database_config {)"
        R"(  host: "localhost")"
        R"(  user: "root")"
        R"(  password: "cai1984")"
        R"(  database: "trading_system")"
        R"(})",
        &config);

    account_pool.reset(new AccountPool());

    event_queue.reset(new Queue<Event*>());
    order_message_queue.reset(new Queue<OrderMessage>());
    strategy_message_queue.reset(new Queue<StrategyMessage>());

    ctp_future_order_client.reset(new CtpFutureOrderClient(
        config, account_pool.get(), event_queue.get(),
        order_message_queue.get(), strategy_message_queue.get()));

    AccountOperator* account_operator =
        new AccountOperator(R"(id: "test_account")");

    account_pool->Insert("test_account", account_operator);
  }

  virtual ~CtpFutureOrderClientFixture() {}

  unique_ptr<AccountPool> account_pool;
  unique_ptr<Queue<OrderMessage>> order_message_queue;
  unique_ptr<Queue<StrategyMessage>> strategy_message_queue;
  unique_ptr<Queue<Event*>> event_queue;
  unique_ptr<CtpFutureOrderClient> ctp_future_order_client;
};

BOOST_FIXTURE_TEST_SUITE(CtpFutureOrderClientTests, CtpFutureOrderClientFixture)

// Success
/*BOOST_AUTO_TEST_CASE(CancelOrderTest) {
  ctp_future_order_client->Init();
  ctp_future_order_client->Start();

        int64_t order_id;
        std::cin >> order_id;

        string wait;
  {
    AccountOperator* account_operator =
        account_pool->GetAccountOperatorByAccountId("test_account");

    Order order;
    TextFormat::ParseFromString(
        R"(name : "rb1705")"
        R"(quantity: 1)"
        R"(price: 3400.0)",
        &order);

    order.set_action(BUY);
    order.set_direction(OPEN);
    order.set_id(order_id);
    account_operator->InsertOrder(order);

    LOG(INFO) << account_operator->GetAccount().DebugString();

                std::cin >> wait;

    OrderMessage order_message;
    TextFormat::ParseFromString(
        R"(account_id: "test_account")"
        R"(name: "rb1705")",
        &order_message);

    order_message.set_command(PLACE_ORDER);
    order_message.set_order_id(order_id);
    order_message.set_command(PLACE_ORDER);
    order_message_queue->Insert(order_message);
    order_message_queue->Notify();

    std::cin >> wait;

    LOG(INFO) << account_operator->GetAccount().DebugString();
  }

  {
    OrderMessage order_message;
    TextFormat::ParseFromString(
        R"(account_id: "test_account")"
        R"(name: "rb1705")",
        &order_message);

    order_message.set_command(CANCEL_ORDER);
    order_message.set_order_id(order_id);
    order_message_queue->Insert(order_message);
    order_message_queue->Notify();
  }

  std::cin >> wait;

  {
    AccountOperator* account_operator =
        account_pool->GetAccountOperatorByAccountId("test_account");

    LOG(INFO) << account_operator->GetAccount().DebugString();
  }

  std::cin >> wait;

  ctp_future_order_client->Exit();
}*/

/*BOOST_AUTO_TEST_CASE(TradedOrderTest) {
  ctp_future_order_client->Init();
  ctp_future_order_client->Start();

  int64_t order_id;
  std::cin >> order_id;

  {
    AccountOperator* account_operator =
        account_pool->GetAccountOperatorByAccountId("test_account");

    Order order;
    TextFormat::ParseFromString(
        R"(name: "rb1705")"
        R"(quantity: 50)"
        R"(price: 3600.0)",
        &order);
    order.set_id(order_id);
    order.set_action(BUY);
    order.set_direction(OPEN);
    account_operator->InsertOrder(order);

    OrderMessage order_message;
    TextFormat::ParseFromString(
        R"(account_id: "test_account")"
        R"(name: "rb1705")",
        &order_message);
    order_message.set_order_id(order_id);
    order_message.set_command(PLACE_ORDER);
    order_message_queue->Insert(order_message);
    order_message_queue->Notify();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    LOG(INFO) << account_operator->GetAccount().DebugString();
  }

  std::cin >> order_id;

  {
    AccountOperator* account_operator =
        account_pool->GetAccountOperatorByAccountId("test_account");

    Order order;
    TextFormat::ParseFromString(
        R"(name: "rb1705")"
        R"(quantity: 50)"
        R"(price: 3400.0)",
        &order);
    order.set_id(order_id);
    order.set_action(SELL);
    order.set_direction(CLOSE_TODAY);
    account_operator->InsertOrder(order);

    OrderMessage order_message;
    TextFormat::ParseFromString(
        R"(account_id: "test_account")"
        R"(name: "rb1705")",
        &order_message);
    order_message.set_order_id(order_id);
    order_message.set_command(PLACE_ORDER);
    order_message_queue->Insert(order_message);
    order_message_queue->Notify();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    LOG(INFO) << account_operator->GetAccount().DebugString();
  }

  string wait;
  std::cin >> wait;

  ctp_future_order_client->Exit();
}*/

BOOST_AUTO_TEST_CASE(RequestInstrumentTest) {
  ctp_future_order_client->Init();
  ctp_future_order_client->Start();

  string wait;
  std::cin >> wait;

  {
    OrderMessage order_message;
    TextFormat::ParseFromString(
        R"(command: REQUEST_SECURITY_DETAIL)", &order_message);

    order_message_queue->Insert(order_message);
    order_message_queue->Notify();
  }

  std::cin >> wait;

  ctp_future_order_client->Exit();
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace order_maker