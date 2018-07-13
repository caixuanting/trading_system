// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "interfaces/signal/signal_base.h"

using google::protobuf::RepeatedPtrField;
using std::string;
using std::vector;
using util::TimeInterval;

namespace interface {

SignalBase::SignalBase(const string& name,
                       const RepeatedPtrField<TimeInterval>& time_intervals)
    : time_intervals_(time_intervals), name_(name) {}

SignalBase::~SignalBase() {}

string SignalBase::Name() { return name_; }

RepeatedPtrField<TimeInterval> SignalBase::TimeIntervals() {
  return time_intervals_;
}

void SignalBase::LookUp(const util::DatabaseConfig& config,
                        const string& command, const string& begin_time) {}

void SignalBase::Dump(const util::DatabaseConfig& config,
                      const string& directory, const string& command,
                      const TimeInterval& time_interval) {}

vector<Signal> SignalBase::GetAllSignals() { return vector<Signal>(); }

Signal SignalBase::GetLastSignal() { return Signal(); }

Signal SignalBase::GetLastSignalWithin(const string& timestamp,
                                       const uint32_t time_in_seconds) {
  return Signal();
}

vector<SignalBase*> SignalBase::DependentSignals() {
  return vector<SignalBase*>();
}

void SignalBase::Update() {}

}  // namespace interface