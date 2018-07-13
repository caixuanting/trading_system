// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef INTERFACES_SIGNAL_SIGNAL_PERIODIC_H_
#define INTERFACES_SIGNAL_SIGNAL_PERIODIC_H_

#include <boost/thread.hpp>

#include "interfaces/signal/signal_base.h"

namespace interface {

class SignalPeriodic : public SignalBase {
 public:
  SignalPeriodic(const std::string& name, const uint32_t num_periods,
                 const uint32_t period_in_seconds,
                 const google::protobuf::RepeatedPtrField<util::TimeInterval>&
                     time_intervals,
                 SignalBase* time_signal);
  virtual ~SignalPeriodic();

  std::vector<Signal> GetAllSignals() override;

  Signal GetLastSignal() override;

  Signal GetLastSignalWithin(const std::string& timestamp,
                             const uint32_t time_in_seconds) override;

  void LookUp(const util::DatabaseConfig& config, const std::string& command,
              const std::string& begin_time) override;

  void Dump(const util::DatabaseConfig& config, const std::string& signal_dir,
            const std::string& command,
            const util::TimeInterval& time_interval) override;

 protected:
  // Not thread safe.
  void Update() override;

  // Not thread safe.
  void WriteToFile(const std::string& signal_dir,
                   const util::TimeInterval& time_interval);

  // Not thread safe.
  void WriteToDatabase(const util::DatabaseConfig& config,
                       const std::string& command,
                       const util::TimeInterval& time_interval);

  // Implement signal computation logic in this method.
  virtual void ComputeNextSignal(
      const boost::posix_time::ptime& next_timestamp);

  SignalBase* time_signal_;
  boost::shared_mutex signal_mutex_;
  uint32_t num_periods_;
  uint32_t period_in_seconds_;
  std::vector<Signal> signals_;
};

}  // namespace interface

#endif  // INTERFACES_SIGNAL_SIGNAL_PERIODIC_H_
