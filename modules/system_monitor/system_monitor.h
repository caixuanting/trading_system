// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef MODULES_SYSTEM_MONITOR_SYSTEM_MONITOR_H_
#define MODULES_SYSTEM_MONITOR_SYSTEM_MONITOR_H_

#include "modules/event_scheduler/event.h"
#include "modules/system_monitor/counter_pool.h"
#include "utils/queue.h"

namespace system_monitor {

class SystemMonitor {
 public:
  SystemMonitor(CounterPool* counter_pool,
                util::Queue<event_scheduler::Event*>* event_queue);
  virtual ~SystemMonitor();

  void Init();
  void Start();
  void ShutDown();

 private:
  void CollectSystemData();
  
  HttpClient http_client_;
  Queue<CHSEvent*>* event_queue_;  // Not owned.
  std::unique_ptr<CHSEvent> event_;
};

}  // namespace system_monitor

#endif  // MODULES_SYSTEM_MONITOR_SYSTEM_MONITOR_H_
