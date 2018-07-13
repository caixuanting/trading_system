// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "modules/order_maker/order_maker.h"

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <google/protobuf/text_format.h>

using event_scheduler::Event;
using google::protobuf::TextFormat;
using interface::AccountOperator;
using interface::AccountPool;
using interface::BUY;
using interface::OPEN;
using interface::Order;
using std::unique_ptr;
using strategy_runner::StrategyMessage;
using util::Queue;

namespace order_maker {

struct OrderMakerFixture {
  OrderMakerFixture() {
    OrderMakerConfig order_maker_config;

    TextFormat::ParseFromString(
        R"(ctp_future_order_client_config {)"
        R"(  name: "test_ctp_future_order_client")"
        R"(  broker_id: "9999")"
        R"(  user_id: "015236")"
        R"(  password: "123456")"
        R"(  frontend_address: "tcp://180.168.146.187:10000")"
        R"(  log_dir: "test/")"
        R"(  trading_interval {)"
        R"(    begin_time: "09:00:00")"
        R"(    end_time: "15:00:00")"
        R"(  })"
        R"(})",
        &order_maker_config);

    account_pool.reset(new AccountPool());

    AccountOperator* account_operator =
        new AccountOperator(R"(id: "test_account")");
    account_pool->Insert("test_account", account_operator);

    order_message_queue.reset(new Queue<OrderMessage>());
    strategy_message_queue.reset(new Queue<StrategyMessage>());
    event_queue.reset(new Queue<Event*>());

    order_maker.reset(new OrderMaker(
        order_maker_config, account_pool.get(), order_message_queue.get(),
        strategy_message_queue.get(), event_queue.get()));
  }

  virtual ~OrderMakerFixture() {}

  unique_ptr<AccountPool> account_pool;
  unique_ptr<Queue<OrderMessage>> order_message_queue;
  unique_ptr<Queue<StrategyMessage>> strategy_message_queue;
  unique_ptr<Queue<Event*>> event_queue;
  unique_ptr<OrderMaker> order_maker;
};

BOOST_FIXTURE_TEST_SUITE(OrderMakerTests, OrderMakerFixture)

BOOST_AUTO_TEST_CASE(EndToEndTest) {
  order_maker->Init();
  order_maker->Start();

  std::this_thread::sleep_for(std::chrono::seconds(2));

  int64_t order_id;
  std::cin >> order_id;

  {
    Order order;
    TextFormat::ParseFromString(
        R"(security_id: "rb1705")"
        R"(quantity: 50)"
        R"(price: 3000.0)",
        &order);
    order.set_id(order_id);
    order.set_action(BUY);
    order.set_direction(OPEN);
    account_pool->GetAccountOperatorByAccountId("test_account")
        ->InsertOrder(order);

    OrderMessage order_message;
    TextFormat::ParseFromString(
        R"(client_name: "test_ctp_future_order_client")"
        R"(account_id: "test_account")"
        R"(name: "rb1705")",
        &order_message);
    order_message.set_order_id(order_id);
    order_message.set_command(PLACE_ORDER);
    order_message_queue->Insert(order_message);
    order_message_queue->Notify();
  }

  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cout << account_pool->GetAccountOperatorByAccountId("test_account")
                   ->GetAccount()
                   .DebugString()
            << std::endl;

  {
    OrderMessage order_message;
    TextFormat::ParseFromString(
        R"(client_name: "test_ctp_future_order_client")"
        R"(account_id: "test_account")"
        R"(name: "rb1705")",
        &order_message);

    order_message.set_command(CANCEL_ORDER);
    order_message.set_order_id(order_id);
    order_message_queue->Insert(order_message);
    order_message_queue->Notify();
  }

  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cout << account_pool->GetAccountOperatorByAccountId("test_account")
                   ->GetAccount()
                   .DebugString()
            << std::endl;

  order_maker->Exit();
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace order_maker