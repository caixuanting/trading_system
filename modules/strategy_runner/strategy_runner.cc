// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "modules/strategy_runner/strategy_runner.h"

#include <glog/logging.h>

#include "modules/strategy_runner/strategies/strategy_test.h"
#include "utils/date_time.h"

using boost::posix_time::microsec_clock;
using event_scheduler::Event;
using interface::AccountPool;
using interface::DataPool;
using interface::OrderFactory;
using interface::SignalPool;
using order_maker::OrderMessage;
using signal_manager::SignalMessage;
using std::string;
using std::thread;
using util::Queue;
using util::ToString;

namespace strategy_runner {

StrategyRunner::StrategyRunner(
    const StrategyRunnerConfig& strategy_runner_config,
    OrderFactory* order_factory, AccountPool* account_pool, DataPool* data_pool,
    SignalPool* signal_pool, Queue<Event*>* event_queue,
    Queue<StrategyMessage>* strategy_message_queue,
    Queue<SignalMessage>* signal_message_queue,
    Queue<OrderMessage>* order_message_queue)
    : strategy_runner_config_(strategy_runner_config),
      strategy_message_queue_(strategy_message_queue),
      event_queue_(event_queue),
      order_message_queue_(order_message_queue),
      signal_message_queue_(signal_message_queue) {
  // Create one strategy message queue for each strategy. Pass in signal info
  // and order info into strategy through strategy message queue.
  for (const auto& config :
       strategy_runner_config.strategy_configs().strategy_config()) {
    LOG(INFO) << "Creating strategy message queue for <" << config.name()
              << ">";

    strategy_name_to_strategy_message_queue_[config.name()].reset(
        new Queue<StrategyMessage>());
  }

  // Register strategy message queue with corresponding data_id.
  for (const auto& config :
       strategy_runner_config.strategy_configs().strategy_config()) {
    for (const auto& data_id : config.data_id()) {
      LOG(INFO) << "Register strategy message queue for <" << config.name()
                << "> to data <" << data_id << ">";

      data_id_to_strategy_message_queues_[data_id].insert(
          strategy_name_to_strategy_message_queue_[config.name()].get());
    }
  }

  // Create strategies.
  for (const auto& config :
       strategy_runner_config.strategy_configs().strategy_config()) {
    LOG(INFO) << "Create strategy <" << config.DebugString() << ">";

    switch (config.type()) {
      case TEST:
        strategy_name_to_strategy_[config.name()].reset(new StrategyTest(
            config, data_pool->GetDataById(config.data_id()[0])->GetSecurity(),
            account_pool->GetAccountOperatorByAccountId(config.account_id()),
            order_factory, event_queue,
            strategy_name_to_strategy_message_queue_[config.name()].get(),
            order_message_queue,
            signal_pool->GetSignalByName(config.ask_price_signal()),
            signal_pool->GetSignalByName(config.bid_price_signal())));
        break;
      default:
        break;
    }
  }
}

StrategyRunner::~StrategyRunner() {}

void StrategyRunner::Init() {
  LOG(INFO) << "Initing strategy runner";

  // Init all strategies.
  for (const auto& strategy_name_to_strategy : strategy_name_to_strategy_) {
    strategy_name_to_strategy.second->Init();
  }

  LOG(INFO) << "Inited strategy runner";
}

void StrategyRunner::Start() {
  LOG(INFO) << "Starting strategy runner";

  // Thread to process info from orders.
  strategy_message_thread_ = thread([this] {
    while (this->running_) {
      this->strategy_message_queue_->Wait();

      if (this->running_) {
        while (!this->strategy_message_queue_->Empty()) {
          StrategyMessage strategy_message =
              this->strategy_message_queue_->Next();
          strategy_message.set_type(ORDER);
          strategy_message.set_timestamp(
              ToString(microsec_clock::local_time()));

          Queue<StrategyMessage>* strategy_message_queue =
              this->strategy_name_to_strategy_message_queue_
                  [strategy_message.strategy_name()]
                      .get();

          strategy_message_queue->Insert(strategy_message);
          strategy_message_queue->Notify();
        }
      }
    }
  });

  // Thread to process info from signals.
  signal_message_thread_ = thread([this] {
    while (this->running_) {
      this->signal_message_queue_->Wait();

      if (this->running_) {
        while (!this->signal_message_queue_->Empty()) {
          SignalMessage signal_message = this->signal_message_queue_->Next();

          string data_id = signal_message.data_id();
          StrategyMessage strategy_message;
          strategy_message.set_type(DATA);
          strategy_message.set_data_id(data_id);
          strategy_message.set_timestamp(
              ToString(microsec_clock::local_time()));
          strategy_message.set_timestamp_from_signal(
              signal_message.timestamp());
          strategy_message.set_timestamp_from_data(
              signal_message.timestamp_from_data());

          for (auto* queue :
               this->data_id_to_strategy_message_queues_[data_id]) {
            queue->Insert(strategy_message);
            queue->Notify();
          }
        }
      }
    }
  });

  LOG(INFO) << "Started strategy runner";
}

void StrategyRunner::Exit() {
  LOG(INFO) << "Exiting strategy runner";

  running_ = false;
  strategy_message_queue_->Notify();
  strategy_message_thread_.join();

  signal_message_queue_->Notify();
  signal_message_thread_.join();

  // Exit all strategies.
  for (const auto& strategy_name_to_strategy : strategy_name_to_strategy_) {
    strategy_name_to_strategy.second->Exit();
  }

  LOG(INFO) << "Exited strategy runner";
}

}  // namespace strategy_runner