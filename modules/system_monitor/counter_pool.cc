// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "modules/system_monitor/counter_pool.h"

using std::lock_guard;
using std::mutex;
using std::string;
	
namespace system_monitor {

CounterPool::CounterPool() {}

CounterPool::~CounterPool() {}

Counter CounterPool::GetCounter(const string& counter_name) {
  lock_guard<mutex> guard(counter_mutex_);

  return name_to_counter_[counter_name];
}

void CounterPool::Increment(const string& counter_name) {
  Increment(counter_name, 1);
}

void CounterPool::Increment(const string& counter_name, const int64_t count) {
  lock_guard<mutex> guard(counter_mutex_);

  name_to_counter_[counter_name].Increment(count);
}

void CounterPool::SetCounter(const string& counter_name, const int64_t count) {
  lock_guard<mutex> guard(counter_mutex_);

  name_to_counter_[counter_name].SetCurrentTotal(count);
}

NameToCounter::iterator CounterPool::begin() {
  return name_to_counter_.begin();
}

NameToCounter::iterator CounterPool::end() {
  return name_to_counter_.end();
}

}  // namespace system_monitor
