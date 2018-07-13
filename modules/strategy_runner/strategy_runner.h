// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef MODULES_STRATEGY_RUNNER_STRATEGY_RUNNER_H_
#define MODULES_STRATEGY_RUNNER_STRATEGY_RUNNER_H_

#include <set>
#include <thread>
#include <unordered_map>

#include "interfaces/account/account_pool.h"
#include "interfaces/data/data_pool.h"
#include "interfaces/signal/signal_pool.h"
#include "modules/order_maker/proto/order_message.pb.h"
#include "modules/signal_manager/proto/signal_message.pb.h"
#include "modules/strategy_runner/proto/strategy_runner_config.pb.h"
#include "modules/strategy_runner/strategies/strategy_base.h"

namespace strategy_runner {

typedef std::unordered_map<std::string,
                           std::unique_ptr<util::Queue<StrategyMessage>>>
    StrategyNameToStrategyMessageQueue;
typedef std::unordered_map<std::string, std::unique_ptr<StrategyBase>>
    StrategyNameToStrategy;
typedef std::unordered_map<std::string, std::set<util::Queue<StrategyMessage>*>>
    DataIdToStrategyMessageQueues;

class StrategyRunner {
 public:
  StrategyRunner(
      const StrategyRunnerConfig& strategy_runner_config,
      interface::OrderFactory* order_factory,
      interface::AccountPool* account_pool, interface::DataPool* data_pool,
      interface::SignalPool* signal_pool,
      util::Queue<event_scheduler::Event*>* event_queue,
      util::Queue<StrategyMessage>* strategy_message_queue,
      util::Queue<signal_manager::SignalMessage>* signal_message_queue,
      util::Queue<order_maker::OrderMessage>* order_message_queue);
  virtual ~StrategyRunner();

  void Init();
  void Start();
  void Exit();

 private:
  DataIdToStrategyMessageQueues data_id_to_strategy_message_queues_;
  StrategyNameToStrategy strategy_name_to_strategy_;
  StrategyNameToStrategyMessageQueue strategy_name_to_strategy_message_queue_;
  StrategyRunnerConfig strategy_runner_config_;
  bool running_ = true;
  std::thread signal_message_thread_;
  std::thread strategy_message_thread_;
  util::Queue<StrategyMessage>* strategy_message_queue_;         // Not owned.
  util::Queue<event_scheduler::Event*>* event_queue_;            // Not owned.
  util::Queue<order_maker::OrderMessage>* order_message_queue_;  // Not owned.
  util::Queue<signal_manager::SignalMessage>*
      signal_message_queue_;  // Not owned.
};

}  // strategy_runner

#endif  // MODULES_STRATEGY_RUNNER_STRATEGY_RUNNER_H_