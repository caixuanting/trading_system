// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "interfaces/signal/signal_periodic.h"

#include <cstdlib>
#include <fstream>

#include <boost/algorithm/string/join.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include "utils/database_operator.h"
#include "utils/date_time.h"

using boost::algorithm::join;
using boost::filesystem::exists;
using boost::gregorian::date;
using boost::gregorian::day_clock;
using boost::posix_time::duration_from_string;
using boost::posix_time::ptime;
using boost::posix_time::seconds;
using boost::posix_time::time_from_string;
using boost::posix_time::to_iso_extended_string;
using boost::shared_lock;
using boost::shared_mutex;
using google::protobuf::RepeatedPtrField;
using google::protobuf::TextFormat;
using sql::ResultSet;
using std::ofstream;
using std::string;
using std::to_string;
using std::unique_lock;
using std::unique_ptr;
using std::vector;
using util::DatabaseConfig;
using util::DatabaseOperator;
using util::LessOrEqual;
using util::Minus;
using util::TimeInterval;
using util::ToString;
using util::Within;
using util::WithinTimeInterval;

namespace interface {

SignalPeriodic::SignalPeriodic(
    const string& name, const uint32_t num_periods,
    const uint32_t period_in_seconds,
    const RepeatedPtrField<TimeInterval>& time_intervals,
    SignalBase* time_signal)
    : SignalBase(name, time_intervals),
      time_signal_(time_signal),
      num_periods_(num_periods),
      period_in_seconds_(period_in_seconds) {}

SignalPeriodic::~SignalPeriodic() {}

vector<Signal> SignalPeriodic::GetAllSignals() {
  shared_lock<shared_mutex> lock(signal_mutex_);

  Update();

  return signals_;
}

Signal SignalPeriodic::GetLastSignal() {
  shared_lock<shared_mutex> lock(signal_mutex_);

  Update();

  if (signals_.empty()) {
    return Signal();
  } else {
    return signals_.back();
  }
}

Signal SignalPeriodic::GetLastSignalWithin(const std::string& timestamp,
                                           const uint32_t time_in_seconds) {
  shared_lock<shared_mutex> lock(signal_mutex_);

  Update();

  for (auto iter = signals_.rbegin(); iter != signals_.rend(); iter++) {
    if (Within(iter->timestamp(), timestamp, time_in_seconds)) {
      return *iter;
    }
  }

  return Signal();
}

void SignalPeriodic::LookUp(const DatabaseConfig& config, const string& command,
                            const string& begin_time) {
  unique_lock<shared_mutex> lock(signal_mutex_);

  DatabaseOperator database_operator(config);

  if (!database_operator.Init()) {
    LOG(ERROR) << "Database init failed";

    return;
  }

  unique_ptr<ResultSet> result_set(database_operator.Read(
      command + " LIMIT " + to_string(num_periods_ - 1) + ";"));

  if (result_set != nullptr) {
    while (result_set->next()) {
      Signal signal;

      TextFormat::ParseFromString(result_set->getString("value"), &signal);

      signals_.insert(signals_.begin(), signal);
    }
  }

  size_t size = signals_.size();

  LOG(INFO) << "Looked up <" << size << "> signals for <" << name_ << ">";

  if (size == 0) {
    Signal signal;

    signal.set_timestamp(ToString(Minus(begin_time, period_in_seconds_)));
    signal.set_is_valid(true);

    signals_.push_back(signal);
  } else {
    for (size_t i = 0; i < size; i++) {
      signals_[i].set_is_valid(true);
      signals_[i].set_timestamp(
          ToString(Minus(begin_time, period_in_seconds_ * (size - i))));
    }
  }
}

void SignalPeriodic::Dump(const DatabaseConfig& config,
                          const string& signal_dir, const string& command,
                          const TimeInterval& time_interval) {
  unique_lock<shared_mutex> lock(signal_mutex_);

  WriteToFile(signal_dir, time_interval);

  WriteToDatabase(config, command, time_interval);

  signals_.clear();
}

void SignalPeriodic::Update() {
  if (signals_.empty()) {
    LOG(ERROR) << "Signal is empty";

    return;
  }
  ptime last_timestamp = time_from_string(signals_.back().timestamp());
  ptime next_timestamp = last_timestamp + seconds(period_in_seconds_);

  Signal latest_signal = time_signal_->GetLastSignal();

  if (!latest_signal.is_valid()) {
    return;
  }

  ptime current_timestamp = time_from_string(latest_signal.string_value());

  while (next_timestamp < current_timestamp) {
    ComputeNextSignal(next_timestamp);

    next_timestamp += seconds(period_in_seconds_);
  }

  LOG(INFO) << "Dumped <" << signals_.size() << "> signals for <" << name_
            << "> to database";
}

void SignalPeriodic::WriteToFile(const string& directory,
                                 const TimeInterval& time_interval) {
  if (!exists(directory)) {
    string command = "mkdir -p " + directory;
    LOG(INFO) << "Creating directory <" << directory << "> status <"
              << system(command.c_str()) << ">";
  }

  string signal_file_name = directory + "/" + time_interval.end_time();

  ofstream signal_file(signal_file_name);

  if (!signal_file.is_open()) {
    LOG(ERROR) << "Could not open file <" << signal_file_name << ">";
    return;
  }

  TextFormat::Printer printer;
  printer.SetSingleLineMode(true);

  for (const Signal& signal : signals_) {
    if (WithinTimeInterval(signal.timestamp(), time_interval)) {
      string signal_string;
      printer.PrintToString(signal, &signal_string);

      signal_file << signal_string << std::endl;
    }
  }

  LOG(INFO) << "Dumped <" << signals_.size() << "> signals for <" << name_
            << "> to file";
}

void SignalPeriodic::WriteToDatabase(const DatabaseConfig& config,
                                     const string& command,
                                     const TimeInterval& time_interval) {
  DatabaseOperator database_operator(config);

  if (!database_operator.Init()) {
    LOG(WARNING) << "Database init failed";
    return;
  }

  vector<string> values;

  TextFormat::Printer printer;
  printer.SetSingleLineMode(true);

  for (const Signal& signal : signals_) {
    if (WithinTimeInterval(signal.timestamp(), time_interval)) {
      string value = "('" + name_ + "', '" + signal.timestamp() + "', '";

      string signal_string;
      printer.PrintToString(signal, &signal_string);

      value += signal_string + "')";

      values.push_back(value);
    }
  }

  if (values.empty()) {
    return;
  }

  string complete_command = command + join(values, ",") + ";";

  database_operator.Write(command);
}

void SignalPeriodic::ComputeNextSignal(const ptime& next_timestamp) {}

}  // namespace interface
