// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com
//
// Usage:
//  1 Insert "event" by calling event_queue->Insert()
//  2 Notify EventScheduler by calling event_queue->Notify()

#ifndef MODULES_EVENT_SCHEDULER_EVENT_SCHEDULER_H_
#define MODULES_EVENT_SCHEDULER_EVENT_SCHEDULER_H_

#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "modules/event_scheduler/event.h"
#include "modules/event_scheduler/proto/event_scheduler_config.pb.h"
#include "utils/queue.h"

namespace event_scheduler {

class EventScheduler {
 public:
  EventScheduler(const EventSchedulerConfig& config,
                 util::Queue<Event*>* event_queue);
  virtual ~EventScheduler();

  // Start io service for processing events.
  void Init();

  // Run events.
  void Start();

  // Stop io service and main thread.
  void Exit();

  size_t EventNum();
  const DescriptionToEvent& GetDescriptionToEvent();

 private:
  DescriptionToEvent description_to_event_;
  EventSchedulerConfig config_;
  bool running_;
  boost::asio::io_service io_service_;
  boost::thread_group work_threads_;
  std::thread main_thread_;
  std::unique_ptr<boost::asio::io_service::work> work_;
  util::Queue<Event*>* event_queue_;  // Not owned.
};

}  // namespace event_scheduler

#endif  // MODULES_EVENT_SCHEDULER_EVENT_SCHEDULER_H_
