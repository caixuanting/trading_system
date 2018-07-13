// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "modules/system_monitor/counter.h"

namespace system_monitor {

Counter::Counter() : current_total_(0), previous_total_(0) {}

Counter::~Counter() {}

int64_t Counter::GetCurrentTotal() const { return current_total_; }

int64_t Counter::GetPreviousTotal() const { return previous_total_; }

int64_t Counter::GetDiff() const { return current_total_ - previous_total_; }

void Counter::Increment(const int64_t value) { current_total_ += value; }

void Counter::SetCurrentTotal(const int64_t value) { current_total_ = value; }

void Counter::Increment() { Increment(1); }

void Counter::UpdatePreviousTotal() { previous_total_ = current_total_; }

}  // namespace system_monitor
