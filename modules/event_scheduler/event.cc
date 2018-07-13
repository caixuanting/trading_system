// Copyright 2016, Xuanting Cai
// Email: caixuanting@hotmail.com

#include "modules/event_scheduler/event.h"

#include <boost/bind.hpp>

#include <glog/logging.h>

using boost::asio::deadline_timer;
using boost::asio::placeholders::error;
using boost::bind;
using boost::posix_time::ptime;
using boost::posix_time::seconds;
using boost::posix_time::second_clock;
using boost::posix_time::time_duration;
using boost::system::error_code;
using std::function;
using std::string;

namespace event_scheduler {

Event::Event(const Action& action, const ptime& start_time,
             const int64_t period_in_seconds, const string& description)
    : action_(action),
      start_time_(start_time),
      period_in_seconds_(period_in_seconds),
      description_(description) {
  routine_ = [this](const error_code& ec, const int64_t period_in_seconds,
                    deadline_timer* timer) {
    if (canceled_) {
      return;
    }

    this->action_();

    if (period_in_seconds > 0) {
      timer->expires_at(timer->expires_at() + seconds(period_in_seconds));
      timer->async_wait(bind(this->routine_, ec, period_in_seconds, timer));
    }
  };
}

Event::~Event() {}

void Event::Start() {
  // If the event is already canceled, don't run the action.
  if (canceled_) {
    LOG(INFO) << "Event <" << description_ << "> is already canceled";

    return;
  }

  // If the timer does not exist, then exit the event.
  if (timer_ == nullptr) {
    LOG(ERROR) << "Event <" << description_ << "> has no deadline timer";

    return;
  }

  // If the time already passed or the time is invalid (not-a-date-time), the
  // event will be run right away.
  time_duration wait_time = seconds(0);

  ptime now = second_clock::local_time();

  if (start_time_.is_not_a_date_time()) {
    LOG(ERROR) << "Start time for <" << description_ << "> is not a valid time";

    return;
  }

  wait_time = start_time_ - now;

  timer_->expires_from_now(wait_time);
  timer_->async_wait(bind(routine_, error, period_in_seconds_, timer_.get()));
}

void Event::Cancel() { canceled_ = true; }

void Event::SetTimer(deadline_timer* timer) { timer_.reset(timer); }

bool Event::GetCanceled() const { return canceled_; }

ptime Event::GetStartTime() const { return start_time_; }

const Routine& Event::GetRoutine() const { return routine_; }

int64_t Event::GetPeriodInSeconds() const { return period_in_seconds_; }

ptime Event::GetExpiresAt() const { return timer_->expires_at(); }

time_duration Event::GetExpiresFromNow() const {
  return timer_->expires_from_now();
}

const string& Event::GetDescription() const { return description_; }

}  // namespace event_scheduler
