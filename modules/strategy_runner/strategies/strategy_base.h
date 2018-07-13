// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef MODULES_STRATEGY_RUNNER_STRATEGIES_STRATEGY_BASE_H_
#define MODULES_STRATEGY_RUNNER_STRATEGIES_STRATEGY_BASE_H_

#include <thread>

#include "interfaces/account/account_operator.h"
#include "interfaces/order/order_factory.h"
#include "modules/event_scheduler/event.h"
#include "modules/order_maker/proto/order_message.pb.h"
#include "modules/strategy_runner/proto/strategy_message.pb.h"
#include "modules/strategy_runner/strategies/proto/strategy_config.pb.h"
#include "utils/queue.h"

namespace strategy_runner {

class StrategyBase {
 public:
  StrategyBase(const StrategyConfig& config,
               interface::AccountOperator* account_operator,
               interface::OrderFactory* order_factory,
               util::Queue<event_scheduler::Event*>* event_queue,
               util::Queue<StrategyMessage>* strategy_message_queue,
               util::Queue<order_maker::OrderMessage>* order_message_queue);

  virtual ~StrategyBase();

  void Init();
  void Start();
  void Exit();

 protected:
  virtual void Run();

  StrategyConfig config_;
  bool running_ = true;
  interface::AccountOperator* account_operator_;  // Not owned.
  interface::OrderFactory* order_factory_;        // Not owned.
  std::thread main_thread_;
  std::vector<std::unique_ptr<event_scheduler::Event>> events_;
  util::Queue<StrategyMessage>* strategy_message_queue_;         // Not owned.
  util::Queue<event_scheduler::Event*>* event_queue_;            // Not owned.
  util::Queue<order_maker::OrderMessage>* order_message_queue_;  // Not owned.
};

}  // namespace strategy_runner

#endif  // MODULES_STRATEGY_RUNNER_STRATEGIES_STRATEGY_BASE_H_