// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef MODULES_SYSTEM_MONITOR_COUNTER_POOL_H_
#define MODULES_SYSTEM_MONITOR_COUNTER_POOL_H_

#include <mutex>
#include <string>
#include <unordered_map>

#include "modules/system_monitor/counter.h"

namespace system_monitor {

typedef std::unordered_map<std::string, Counter> NameToCounter;
	
class CounterPool {
 public:
  CounterPool();
  virtual ~CounterPool();

  Counter GetCounter(const std::string& counter_name);
  void Increment(const std::string& counter_name);
  void Increment(const std::string& counter_name, const int64_t count);
  void SetCounter(const std::string& counter_name, const int64_t count);

  NameToCounter::iterator begin();
  NameToCounter::iterator end();

 private:
  NameToCounter name_to_counter_;
  std::mutex counter_mutex_;
};

}  // namespace system_monitor

#endif  // MODULES_SYSTEM_MONITOR_COUNTER_POOL_H_
