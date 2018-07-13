// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef MODULES_EVENT_SCHEDULER_EVENT_H_
#define MODULES_EVENT_SCHEDULER_EVENT_H_

#include <functional>
#include <unordered_map>

#include <boost/asio.hpp>

namespace event_scheduler {

typedef std::function<void()> Action;

typedef std::function<void(const boost::system::error_code&, const int64_t,
                           boost::asio::deadline_timer*)>
    Routine;

class Event {
 public:
  Event(const Action& action, const boost::posix_time::ptime& start_time,
        const int64_t period_in_seconds, const std::string& description);

  virtual ~Event();

  void Start();
  void Cancel();

  void SetTimer(boost::asio::deadline_timer* timer);

  bool GetCanceled() const;
  boost::posix_time::ptime GetStartTime() const;
  int64_t GetPeriodInSeconds() const;
  boost::posix_time::ptime GetExpiresAt() const;
  boost::posix_time::time_duration GetExpiresFromNow() const;
  const Routine& GetRoutine() const;
  const std::string& GetDescription() const;

 private:
  Action action_;
  bool canceled_ = false;
  boost::posix_time::ptime start_time_;
  int64_t period_in_seconds_;
  Routine routine_;
  std::string description_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

typedef std::unordered_map<std::string, Event*> DescriptionToEvent;

}  // namespace event_scheduler

#endif  // MODULES_EVENT_SCHEDULER_EVENT_H_
