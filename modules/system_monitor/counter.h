// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef MODULES_SYSTEM_MONITOR_COUNTER_H_
#define MODULES_SYSTEM_MONITOR_COUNTER_H_

#include <cstdint>

namespace system_monitor {

class Counter {
 public:
  Counter();
  virtual ~Counter();

  int64_t GetCurrentTotal() const;
  int64_t GetPreviousTotal() const;
  int64_t GetDiff() const;
  void Increment();
  void Increment(const int64_t value);
  void SetCurrentTotal(const int64_t value);
  void UpdatePreviousTotal();

 private:
  int64_t current_total_;
  int64_t previous_total_;
};

}  // namespace system_monitor

#endif  // MODULES_SYSTEM_MONITOR_COUNTER_H_
