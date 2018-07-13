// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef MODULES_STRATEGY_RUNNER_STRATEGIES_STRATEGY_TEST_H_
#define MODULES_STRATEGY_RUNNER_STRATEGIES_STRATEGY_TEST_H_

#include "interfaces/data/proto/security.pb.h"
#include "interfaces/signal/signal_base.h"
#include "modules/strategy_runner/strategies/strategy_base.h"

namespace strategy_runner {

class StrategyTest : public StrategyBase {
 public:
  StrategyTest(const StrategyConfig& config,
               const interface::Security& security,
               interface::AccountOperator* account_operator,
               interface::OrderFactory* order_factory,
               util::Queue<event_scheduler::Event*>* event_queue,
               util::Queue<StrategyMessage>* strategy_message_queue,
               util::Queue<order_maker::OrderMessage>* order_message_queue,
               interface::SignalBase* ask_price_signal,
               interface::SignalBase* bid_price_signal);

  virtual ~StrategyTest();

 protected:
  void Run() override;
  void ProcessDataMessage();
  void ProcessOrderMessage();

 private:
  interface::Security security_;
  interface::SignalBase* ask_price_signal_;  // Not owned.
  interface::SignalBase* bid_price_signal_;  // Not owned.
};

}  // namespace strategy_runner

#endif  // MODULES_STRATEGY_RUNNER_STRATEGIES_STRATEGY_TEST_H_