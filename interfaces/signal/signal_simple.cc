// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "interfaces/signal/signal_simple.h"

using google::protobuf::RepeatedPtrField;
using std::string;
using std::vector;
using util::TimeInterval;

namespace interface {

SignalSimple::SignalSimple(const string& name,
                           const RepeatedPtrField<TimeInterval>& time_intervals,
                           Data* data, DataType data_type)
    : SignalBase(name, time_intervals), data_(data), data_type_(data_type) {}

SignalSimple::~SignalSimple() {}

Signal SignalSimple::GetLastSignal() {
  Snapshot snapshot = data_->GetLastSnapshot();

  return GetSignalFromSnapshot(snapshot);
}

Signal SignalSimple::GetLastSignalWithin(const string& timestamp,
                                         const uint32_t time_in_seconds) {
  Snapshot snapshot = data_->GetLastSnapshotWithin(timestamp, time_in_seconds);

  return GetSignalFromSnapshot(snapshot);
}

Signal SignalSimple::GetSignalFromSnapshot(const Snapshot& snapshot) {
  Signal signal;

  signal.set_timestamp(snapshot.exchange_time());
  signal.set_is_valid(true && snapshot.is_valid());

  switch (data_type_) {
    case LEVEL_1_BID_PRICE:
      signal.set_value_type(DOUBLE);
      signal.set_double_value(snapshot.level_1_bid_price());
      break;
    case LEVEL_1_BID_VOLUME:
      signal.set_value_type(INT64);
      signal.set_double_value(snapshot.level_1_bid_volume());
      break;
    case LEVEL_1_ASK_PRICE:
      signal.set_value_type(DOUBLE);
      signal.set_double_value(snapshot.level_1_ask_price());
      break;
    case LEVEL_1_ASK_VOLUME:
      signal.set_value_type(INT64);
      signal.set_double_value(snapshot.level_1_ask_volume());
      break;
    case LAST_PRICE:
      signal.set_value_type(DOUBLE);
      signal.set_double_value(snapshot.last_price());
      break;
    case TOTAL_VOLUME:
      signal.set_value_type(INT64);
      signal.set_double_value(snapshot.total_volume());
      break;
    case HIGHEST_PRICE:
      signal.set_value_type(DOUBLE);
      signal.set_double_value(snapshot.highest_price());
      break;
    case LOWEST_PRICE:
      signal.set_value_type(DOUBLE);
      signal.set_double_value(snapshot.lowest_price());
      break;
    case UPPER_LIMIT_PRICE:
      signal.set_value_type(DOUBLE);
      signal.set_double_value(snapshot.upper_limit_price());
      break;
    case LOWER_LIMIT_PRICE:
      signal.set_value_type(DOUBLE);
      signal.set_double_value(snapshot.lower_limit_price());
      break;
    case OPEN_INTEREST:
      signal.set_value_type(INT64);
      signal.set_double_value(snapshot.open_interest());
      break;
    case COUNT:
      signal.set_value_type(INT64);
      signal.set_double_value(snapshot.count());
      break;
    case TIMESTAMP:
      signal.set_value_type(STRING);
      signal.set_string_value(snapshot.exchange_time());
      break;
    default:
      signal.set_is_valid(false);
      break;
  }

  return signal;
}

}  // namespace interface
