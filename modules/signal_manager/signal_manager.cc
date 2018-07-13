// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "modules/signal_manager/signal_manager.h"

#include <algorithm>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <glog/logging.h>

#include "utils/date_time.h"
#include "utils/graph.h"

using boost::gregorian::date;
using boost::gregorian::day_clock;
using boost::posix_time::duration_from_string;
using boost::posix_time::microsec_clock;
using boost::posix_time::ptime;
using boost::posix_time::seconds;
using boost::posix_time::time_duration;
using boost::posix_time::to_simple_string;
using data_receiver::DataMessage;
using event_scheduler::Action;
using event_scheduler::Event;
using interface::SignalBase;
using interface::SignalConfig;
using interface::SignalPool;
using std::reverse;
using std::string;
using std::thread;
using std::unique_ptr;
using util::CreateTimeInterval;
using util::Graph;
using util::LessOrEqual;
using util::Queue;
using util::SerializedGraph;
using util::TimeInterval;
using util::ToString;

namespace signal_manager {
namespace {

const int32_t kSecondsPerDay = 24 * 3600;
const int32_t kTimeToWaitInSeconds = 6 * 60;

}  // namespace

SignalManager::SignalManager(const SignalManagerConfig& config,
                             SignalPool* signal_pool,
                             Queue<Event*>* event_queue,
                             Queue<DataMessage>* data_message_queue,
                             Queue<SignalMessage>* signal_message_queue)
    : config_(config),
      signal_pool_(signal_pool),
      signal_message_queue_(signal_message_queue),
      data_message_queue_(data_message_queue),
      event_queue_(event_queue) {}

SignalManager::~SignalManager() {}

void SignalManager::Init() {
  LOG(INFO) << "Initing signal manager";

  date today = day_clock::local_day();

  // Schedule look up events.
  for (const auto& time_to_graph : signal_pool_->GetBeginGraph()) {
    // Get begin time.
    string time_string = time_to_graph.first;

    // Schedule LookUp events for signals.
    time_duration begin_time = duration_from_string(time_string);
    ptime lookup_start_time(today, begin_time);

    Graph graph = time_to_graph.second;

    Action lookup_action = [this, graph, begin_time, time_string] {
      date day = day_clock::local_day();
      ptime bt(day, begin_time);

      // Look up signals for all nodes in a serialized tree.
      // The lookup is ordered from leaf to root.
      for (const auto& node : graph.SerializeGraph()) {
        SignalBase* signal = this->signal_pool_->GetSignalByName(node);

        if (signal == nullptr) {
          LOG(ERROR) << "Signal <" << node << "> does not exist";

          continue;
        }

        SignalConfig signal_config =
            this->signal_pool_->GetSignalConfigByName(node);

        TimeInterval current_time_interval;

        for (const auto& time_interval : signal_config.time_interval()) {
          if (time_interval.begin_time() == time_string) {
            current_time_interval = CreateTimeInterval(
                time_interval.begin_time(), time_interval.end_time(), day);

            break;
          }
        }

        string now = ToString(microsec_clock::local_time());

        // Only start signal look up if we have not passed the dump time.
        if (LessOrEqual(now, current_time_interval.end_time())) {
          signal->LookUp(this->config_.database_config(),
                         signal_config.lookup_command(), ToString(bt));

          LOG(INFO) << "Finished lookup event for signal <" << node << "> at <"
                    << to_simple_string(begin_time) << ">";
        } else {
          LOG(ERROR) << "Lookup event for signal <" << node << "> at <"
                     << to_simple_string(begin_time) << "> already passed";
        }
      }
    };

    unique_ptr<Event> lookup_event(new Event(
        lookup_action, lookup_start_time - seconds(kTimeToWaitInSeconds),
        kSecondsPerDay, "Look up signals at <" + time_string + ">"));

    events_.emplace_back(move(lookup_event));

    event_queue_->Insert(events_.back().get());
    event_queue_->Notify();
  }

  // Schedule dump events.
  for (const auto& time_to_graph : signal_pool_->GetEndGraph()) {
    string time_string = time_to_graph.first;

    // Schedule Dump events for signals.
    time_duration end_time = duration_from_string(time_string);
    ptime dump_start_time(today, end_time);

    SerializedGraph serialized_graph = time_to_graph.second.SerializeGraph();
    reverse(serialized_graph.begin(), serialized_graph.end());

    Action dump_action = [this, serialized_graph, end_time, time_string] {
      date day = day_clock::local_day();

      for (const auto& node : serialized_graph) {
        SignalBase* signal = this->signal_pool_->GetSignalByName(node);

        if (signal == nullptr) {
          LOG(ERROR) << "Signal <" << node << "> does not exist";

          continue;
        }

        SignalConfig signal_config =
            this->signal_pool_->GetSignalConfigByName(node);

        TimeInterval current_time_interval;

        for (const auto& time_interval : signal_config.time_interval()) {
          if (time_interval.end_time() == time_string) {
            current_time_interval = CreateTimeInterval(
                time_interval.begin_time(), time_interval.end_time(), day);

            break;
          }
        }

        signal->Dump(this->config_.database_config(),
                     this->config_.signal_dir(), signal_config.dump_command(),
                     current_time_interval);

        LOG(INFO) << "Finished dump event for signal <" << node << "> at <"
                  << to_simple_string(end_time) << ">";
      }
    };

    unique_ptr<Event> dump_event(
        new Event(dump_action, dump_start_time + seconds(kTimeToWaitInSeconds),
                  kSecondsPerDay, "Dump signals at <" + time_string + ">"));

    events_.emplace_back(move(dump_event));

    event_queue_->Insert(events_.back().get());
    event_queue_->Notify();
  }

  LOG(INFO) << "Inited signal manager";
}

void SignalManager::Start() {
  LOG(INFO) << "Starting signal manager";

  message_thread_ = thread([this] {
    while (this->running_) {
      this->data_message_queue_->Wait();

      if (this->running_) {
        while (!this->data_message_queue_->Empty()) {
          DataMessage data_message = this->data_message_queue_->Next();

          SignalMessage signal_message;

          signal_message.set_data_id(data_message.data_id());
          signal_message.set_timestamp(ToString(microsec_clock::local_time()));
          signal_message.set_timestamp_from_data(data_message.timestamp());

          this->signal_message_queue_->Insert(signal_message);
          this->signal_message_queue_->Notify();
        }
      }
    }
  });

  LOG(INFO) << "Started signal manager";
}

void SignalManager::Exit() {
  LOG(INFO) << "Exiting signal manager";

  running_ = false;
  data_message_queue_->Notify();
  message_thread_.join();

  for (const auto& event : events_) {
    event->Cancel();
  }

  LOG(INFO) << "Exited signal manager";
}

}  // namespace signal_manager