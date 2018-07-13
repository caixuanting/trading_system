// Copyright 2016, Xuanting Cai

#include <string>
#include <sys/resource.h>

#include "./chs_monitor.h"

namespace Module {

CHSMonitor::CHSMonitor(CHSStatsPool* stats_pool,
                       CHSQueue<CHSEvent*>* event_queue)
    : CHSModule(stats_pool), event_queue_(event_queue) {
  CHECK(event_queue_);
}

CHSMonitor::~CHSMonitor() {}

void CHSMonitor::init() {
  LOG(info) << "Initializing monitor";

  LOG(info) << "Initialized monitor";
}

void CHSMonitor::start() {
  LOG(info) << "Starting monitor";

  event_.reset(new CHSEvent(
      [this](const error_code& err_code, CHSTimeDuration time_duration,
             deadline_timer* t) {
        this->collectSystemData();

        // Time difference between GMT and Beijing is 8 hours.
        int64_t time_in_seconds =
            CHSClock::getNowLocal().getDateTimeInSeconds() - 8 * 3600;

        for (auto iter = stats_pool_->begin(); iter != stats_pool_->end();
             iter++) {
          CHSCounter& counter = iter->second;
          int64_t value =
              counter.getCurrentTotal() - counter.getPreviousTotal();
          counter.updatePreviousTotal();
          string message = "trading_system." + global_flag.system_instance +
                           "." + iter->first + " " + to_string(value) + " " +
                           to_string(time_in_seconds) + "\n";

          this->http_client_.connectSocket();
          this->http_client_.send(message);
          this->http_client_.disconnectSocket();
        }

        t->expires_at(t->expires_at() + seconds(60));
        t->async_wait(
            bind(this->event_->getRoutine(), err_code, time_duration, t));
      },
      CHSClock::getNowLocal().atMinuteLevel().getTime().toString(),
      "send_stats_to_carbon_server"));

  unique_lock<mutex> lock(event_queue_->getMutex());
  event_queue_->insert(event_.get());
  event_queue_->notify();

  LOG(info) << "Started monitor";
}

void CHSMonitor::shutDown() {
  LOG(info) << "Shutting down monitor";

  LOG(info) << "Shut down monitor";
}

void CHSMonitor::collectSystemData() {
  // Get memory and usage.
  rusage r_usage;
  getrusage(RUSAGE_SELF, &r_usage);
  stats_pool_->increment("system.memory", r_usage.ru_maxrss);
  stats_pool_->increment("system.cpu_user_usage", r_usage.ru_utime.tv_usec);
  stats_pool_->increment("system.cpu_system_usage", r_usage.ru_stime.tv_usec);
}

}  // namespace Module
