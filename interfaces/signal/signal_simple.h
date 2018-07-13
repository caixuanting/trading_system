// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef INTERFACES_SIGNAL_SIGNAL_SIMPLE_H_
#define INTERFACES_SIGNAL_SIGNAL_SIMPLE_H_

#include "interfaces/data/data.h"
#include "interfaces/signal/signal_base.h"

namespace interface {

class SignalSimple : public SignalBase {
 public:
  SignalSimple(const std::string& name,
               const google::protobuf::RepeatedPtrField<util::TimeInterval>&
                   time_intervals,
               Data* data, DataType data_type);
  virtual ~SignalSimple();

  Signal GetLastSignal() override;

  Signal GetLastSignalWithin(const std::string& timestamp,
                             const uint32_t time_in_seconds) override;

 private:
  Signal GetSignalFromSnapshot(const Snapshot& snapshot);

  Data* data_;
  DataType data_type_;
};

}  // namespace interface

#endif  // INTERFACES_SIGNAL_SIGNAL_SIMPLE_H_
