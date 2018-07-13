// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

// This queue is thread safe. All actions are locked.
// To use this queue, class Type must have a default constructor.

#ifndef UTILS_QUEUE_H_
#define UTILS_QUEUE_H_

#include <condition_variable>
#include <mutex>
#include <queue>

namespace util {

template <class Type>
class Queue {
 public:
  Queue() {}
  virtual ~Queue() {}

  void Insert(Type value) {
    std::unique_lock<std::mutex> lock(value_mutex_);
    values_.push(value);
  }

  Type Next() {
    std::unique_lock<std::mutex> lock(value_mutex_);
    Type value;

    if (values_.empty()) {
      return value;
    } else {
      value = values_.front();
      values_.pop();
      return value;
    }
  }

  bool Empty() {
    std::unique_lock<std::mutex> lock(value_mutex_);
    return values_.empty();
  }

  void Wait() {
    std::unique_lock<std::mutex> lock(value_mutex_);
    value_condition_.wait(lock, [this] { return this->signaled_; });

    signaled_ = false;
  }

  void Notify() {
    std::unique_lock<std::mutex> lock(value_mutex_);
    signaled_ = true;
    value_condition_.notify_all();
  }

  void Clear() {
    std::unique_lock<std::mutex> lock(value_mutex_);
    values_ = std::queue<Type>();
  }

  int32_t Size() {
    std::unique_lock<std::mutex> lock(value_mutex_);
    return values_.size();
  }

 private:
  bool signaled_ = false;
  std::condition_variable value_condition_;
  std::mutex value_mutex_;
  std::queue<Type> values_;
};

}  // namespace util

#endif  // UTILS_QUEUE_H_
