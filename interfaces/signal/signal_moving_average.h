// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef INTERFACES_SIGNAL_SIGNAL_MOVING_AVERAGE_H_
#define INTERFACES_SIGNAL_SIGNAL_MOVING_AVERAGE_H_

#include "interfaces/signal/signal_periodic.h"

namespace interface {

class SignalMovingAverage : public SignalPeriodic {
 public:
  SignalMovingAverage(const std::string& name, const int32_t num_periods,
                      const int32_t period_in_seconds,
                      const google::protobuf::RepeatedPtrField<
                          util::TimeInterval>& time_intervals,
                      SignalBase* time_signal, SignalBase* data_signal);
  virtual ~SignalMovingAverage();

  std::vector<SignalBase*> DependentSignals() override;

 private:
  void ComputeNextSignal(
      const boost::posix_time::ptime& next_timestamp) override;

  SignalBase* data_signal_;  // Not owned.
};

}  // namespace interface

#endif  // INTERFACES_SIGNAL_SIGNAL_MOVING_AVERAGE_H_
