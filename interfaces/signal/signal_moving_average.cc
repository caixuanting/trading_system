// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "interfaces/signal/signal_moving_average.h"

#include <glog/logging.h>

#include "utils/date_time.h"

using boost::posix_time::ptime;
using boost::posix_time::seconds;
using google::protobuf::RepeatedPtrField;
using std::vector;
using util::TimeInterval;
using util::ToString;

namespace interface {

SignalMovingAverage::SignalMovingAverage(
    const std::string& name, const int32_t num_periods,
    const int32_t period_in_seconds,
    const RepeatedPtrField<TimeInterval>& time_intervals,
    SignalBase* time_signal, SignalBase* data_signal)
    : SignalPeriodic(name, num_periods, period_in_seconds, time_intervals,
                     time_signal),
      data_signal_(data_signal) {}

SignalMovingAverage::~SignalMovingAverage() {}

vector<SignalBase*> SignalMovingAverage::DependentSignals() {
  return {data_signal_};
}

void SignalMovingAverage::ComputeNextSignal(const ptime& next_timestamp) {
  Signal last_signal = signals_.back();

  vector<Signal> base_signals;

  for (const auto& base_signal : last_signal.base_signal()) {
    base_signals.push_back(base_signal);
  }

  Signal current_base_signal = data_signal_->GetLastSignalWithin(
      ToString(next_timestamp + seconds(period_in_seconds_)),
      period_in_seconds_);

  if (!current_base_signal.is_valid()) {
    LOG(WARNING) << "Current base signal is not valid";

    last_signal.set_timestamp(ToString(next_timestamp));

    signals_.push_back(last_signal);
    return;
  }

  if (base_signals.size() == num_periods_) {
    base_signals.push_back(current_base_signal);
    base_signals.erase(base_signals.begin());
  } else {
    base_signals.push_back(current_base_signal);
  }

  Signal next_signal;

  double sum = 0;

  for (const auto base_signal : base_signals) {
    switch (base_signal.value_type()) {
      case INT32:
        sum += base_signal.int32_value();
        break;
      case INT64:
        sum += base_signal.int64_value();
        break;
      case DOUBLE:
        sum += base_signal.double_value();
        break;
      default:
        break;
    }

    *next_signal.add_base_signal() = base_signal;
  }

  next_signal.set_is_valid(true);
  next_signal.set_timestamp(ToString(next_timestamp));
  next_signal.set_value_type(DOUBLE);
  next_signal.set_double_value(sum / base_signals.size());
  signals_.push_back(next_signal);
}

}  // namespace interface
