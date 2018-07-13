// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "modules/strategy_runner/strategies/strategy_test.h"

#include <glog/logging.h>

using event_scheduler::Event;
using interface::Account;
using interface::AccountOperator;
using interface::BUY;
using interface::CLOSE_TODAY;
using interface::OPEN;
using interface::Order;
using interface::OrderFactory;
using interface::SELL;
using interface::SignalBase;
using order_maker::OrderMessage;
using order_maker::PLACE_ORDER;
using order_maker::CANCEL_ORDER;
using std::vector;
using util::Queue;

namespace strategy_runner {

StrategyTest::StrategyTest(
    const StrategyConfig& config, const interface::Security& security,
    AccountOperator* account_operator, OrderFactory* order_factory,
    Queue<Event*>* event_queue, Queue<StrategyMessage>* strategy_message_queue,
    Queue<OrderMessage>* order_message_queue, SignalBase* ask_price_signal,
    SignalBase* bid_price_signal)
    : StrategyBase(config, account_operator, order_factory, event_queue,
                   strategy_message_queue, order_message_queue),
      security_(security),
      ask_price_signal_(ask_price_signal),
      bid_price_signal_(bid_price_signal) {}

StrategyTest::~StrategyTest() {}

void StrategyTest::Run() {
  while (running_) {
    strategy_message_queue_->Wait();

    if (running_) {
      StrategyMessage strategy_message = strategy_message_queue_->Next();

      switch (strategy_message.type()) {
        case DATA:
          ProcessDataMessage();
          break;
        case ORDER:
          ProcessOrderMessage();
          break;
        default:
          break;
      }
    }
  }

  account_operator_->Lock();
  vector<int64_t> order_ids = account_operator_->FindPendingOrders();

  for (const auto& order_id : order_ids) {
    OrderMessage order_message;

    order_message.set_command(CANCEL_ORDER);
    order_message.set_account_id(account_operator_->GetAccountId());
    order_message.set_order_id(order_id);
    order_message.set_client_name("YIN_HE_CTP_FUTURE");

    order_message_queue_->Insert(order_message);
    order_message_queue_->Notify();
  }

  account_operator_->Unlock();
}

void StrategyTest::ProcessDataMessage() {
  account_operator_->Lock();

  Account account = account_operator_->GetAccount();

  if (account.security_id_to_position_pool().size() == 0 &&
      account.order_id_to_order().size() == 0 &&
      account.order_id_to_executed_orders().size() == 0) {
    Order order = order_factory_->GetANewOrder();

    order.set_security_id(security_.id());
    order.set_name(security_.name());

    order.set_action(BUY);
    order.set_quantity(1);
    order.set_price(bid_price_signal_->GetLastSignal().double_value());

    order.set_direction(OPEN);

    account_operator_->InsertOrder(order);

    OrderMessage order_message;

    order_message.set_command(PLACE_ORDER);
    order_message.set_account_id(account_operator_->GetAccountId());
    order_message.set_order_id(order.id());
    order_message.set_client_name("YIN_HE_CTP_FUTURE");

    order_message_queue_->Insert(order_message);
    order_message_queue_->Notify();
  } else if (account.security_id_to_position_pool().size() != 0 &&
             account.order_id_to_order().size() == 0 &&
             account.order_id_to_executed_orders().size() == 0) {
    Order order = order_factory_->GetANewOrder();

    order.set_security_id(security_.id());
    order.set_name(security_.name());

    order.set_action(SELL);
    order.set_quantity(1);
    order.set_price(ask_price_signal_->GetLastSignal().double_value());

    order.set_direction(CLOSE_TODAY);

    account_operator_->InsertOrder(order);

    OrderMessage order_message;

    order_message.set_command(PLACE_ORDER);
    order_message.set_account_id(account_operator_->GetAccountId());
    order_message.set_order_id(order.id());
    order_message.set_client_name("YIN_HE_CTP_FUTURE");

    order_message_queue_->Insert(order_message);
    order_message_queue_->Notify();
  }

  account_operator_->Unlock();
}

void StrategyTest::ProcessOrderMessage() {
  account_operator_->Lock();

  LOG(INFO) << account_operator_->GetAccount().DebugString();

  account_operator_->Unlock();
}

}  // namespace strategy_runner