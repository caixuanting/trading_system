// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "modules/strategy_runner/strategies/strategy_base.h"

#include <boost/test/unit_test.hpp>

#include <google/protobuf/text_format.h>

using event_scheduler::Event;
using order_maker::OrderMessage;
using std::unique_ptr;
using util::Queue;

namespace strategy_runner {

struct StrategyBaseFixture {
  StrategyBaseFixture() {
    StrategyConfig config;
    event_queue.reset(new Queue<Event*>());
    order_message_queue.reset(new Queue<OrderMessage>());
    strategy_message_queue.reset(new Queue<StrategyMessage>());
    strategy.reset(new StrategyBase(config, nullptr, event_queue.get(),
                                    strategy_message_queue.get(),
                                    order_message_queue.get()));
  }

  virtual ~StrategyBaseFixture() {}

  unique_ptr<Queue<Event*>> event_queue;
  unique_ptr<Queue<OrderMessage>> order_message_queue;
  unique_ptr<Queue<StrategyMessage>> strategy_message_queue;
  unique_ptr<StrategyBase> strategy;
};

BOOST_FIXTURE_TEST_SUITE(StrategyBaseTests, StrategyBaseFixture)

BOOST_AUTO_TEST_CASE(EndToEndTest) {
  strategy->Init();
  strategy->Start();

  std::this_thread::sleep_for(std::chrono::seconds(1));

  StrategyMessage strategy_message;
  strategy_message_queue->Insert(strategy_message);
  strategy_message_queue->Notify();

  strategy->Exit();
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace strategy_runner