// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

// This is the base class for all signal classes.
// Every signal class has to inherit from this class.

#ifndef INTERFACES_SIGNAL_SIGNAL_BASE_H_
#define INTERFACES_SIGNAL_SIGNAL_BASE_H_

#include <string>
#include <unordered_map>
#include <vector>

#include <google/protobuf/repeated_field.h>

#include "interfaces/data/proto/security.pb.h"
#include "interfaces/signal/proto/signal.pb.h"
#include "utils/proto/database_config.pb.h"

namespace interface {

class SignalBase {
 public:
  SignalBase(const std::string& name,
             const google::protobuf::RepeatedPtrField<util::TimeInterval>&
                 time_intervals);
  virtual ~SignalBase();

  std::string Name();
  google::protobuf::RepeatedPtrField<util::TimeInterval> TimeIntervals();

  virtual void LookUp(const util::DatabaseConfig& config,
                      const std::string& command,
                      const std::string& begin_time);

  virtual void Dump(const util::DatabaseConfig& config,
                    const std::string& directory, const std::string& command,
                    const util::TimeInterval& time_interval);

  virtual std::vector<Signal> GetAllSignals();

  virtual Signal GetLastSignal();

  // Get last signal within [timestamp - time_in_seconds, timestamp).
  virtual Signal GetLastSignalWithin(const std::string& timestamp,
                                     const uint32_t time_in_seconds);

  virtual std::vector<SignalBase*> DependentSignals();

 protected:
  virtual void Update();

  google::protobuf::RepeatedPtrField<util::TimeInterval> time_intervals_;
  std::string name_;
};

}  // namespace interface

#endif  // INTERFACES_SIGNAL_SIGNAL_SIMPLE_H_