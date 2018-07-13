// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "modules/strategy_runner/strategies/strategy_base.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <glog/logging.h>

#include "utils/date_time.h"

using boost::gregorian::date;
using boost::gregorian::day_clock;
using boost::posix_time::duration_from_string;
using boost::posix_time::ptime;
using boost::posix_time::seconds;
using boost::posix_time::time_duration;
using boost::posix_time::to_simple_string;
using event_scheduler::Action;
using event_scheduler::Event;
using interface::AccountOperator;
using interface::OrderFactory;
using order_maker::OrderMessage;
using std::move;
using std::thread;
using std::unique_ptr;
using util::Queue;

namespace strategy_runner {

const int32_t kSecondsPerDay = 24 * 3600;

StrategyBase::StrategyBase(const StrategyConfig& config,
                           AccountOperator* account_operator,
                           OrderFactory* order_factory,
                           Queue<Event*>* event_queue,
                           Queue<StrategyMessage>* strategy_message_queue,
                           Queue<OrderMessage>* order_message_queue)
    : config_(config),
      account_operator_(account_operator),
      order_factory_(order_factory),
      strategy_message_queue_(strategy_message_queue),
      event_queue_(event_queue),
      order_message_queue_(order_message_queue) {}

StrategyBase::~StrategyBase() {
  for (auto& event : events_) {
    event->Cancel();
  }
}

void StrategyBase::Init() {
  LOG(INFO) << "Initing strategy <" << config_.name() << ">";

  date today = day_clock::local_day();

  // Create "Start" and "Exit" events.
  for (const auto& time_interval : config_.time_interval()) {
    time_duration begin_time = duration_from_string(time_interval.begin_time());
    ptime begin_start_time(today, begin_time);

    Action begin_action = [this, begin_time] {
      LOG(INFO) << "Start strategy <" << config_.name() << "> at <"
                << to_simple_string(begin_time) << ">";
      this->Start();
    };

    unique_ptr<Event> begin_event(
        new Event(begin_action, begin_start_time, kSecondsPerDay,
                  "Start strategy for <" + config_.name() + "> at <" +
                      to_simple_string(begin_time) + ">"));
    events_.emplace_back(move(begin_event));

    event_queue_->Insert(events_.back().get());
    event_queue_->Notify();

    time_duration end_time = duration_from_string(time_interval.end_time());
    ptime end_start_time(today, end_time);

    Action end_action = [this, end_time] {
      LOG(INFO) << "Exit strategy <" << config_.name() << "> at <"
                << to_simple_string(end_time) << ">";
      this->Exit();
    };

    unique_ptr<Event> end_event(
        new Event(end_action, end_start_time, kSecondsPerDay,
                  "Exit strategy <" + config_.name() + "> at <" +
                      to_simple_string(end_time) + ">"));
    events_.emplace_back(move(end_event));

    event_queue_->Insert(events_.back().get());
    event_queue_->Notify();
  }

  LOG(INFO) << "Inited strategy <" << config_.name() << ">";
}

void StrategyBase::Start() {
  LOG(INFO) << "Starting strategy <" << config_.name() << ">";

  running_ = true;

  main_thread_ = thread([this] { this->Run(); });

  LOG(INFO) << "Started strategy <" << config_.name() << ">";
}

void StrategyBase::Exit() {
  LOG(INFO) << "Exiting strategy <" << config_.name() << ">";

  running_ = false;
  strategy_message_queue_->Notify();

  if (main_thread_.joinable()) {
    main_thread_.join();
  }

  strategy_message_queue_->Clear();

  LOG(INFO) << "Exited strategy <" << config_.name() << ">";
}

void StrategyBase::Run() {
  while (running_) {
    strategy_message_queue_->Wait();

    if (running_) {
      LOG(INFO) << "Running base strategy";

      while (!strategy_message_queue_->Empty()) {
        StrategyMessage strategy_message = strategy_message_queue_->Next();
        LOG(INFO) << strategy_message.DebugString();

        if (strategy_message.type() == ORDER) {
          LOG(INFO) << "Order type strategy message";
        } else {
          LOG(INFO) << "Data type strategy message";
        }
      }
    }
  }
}

}  // namespace strategy_runner