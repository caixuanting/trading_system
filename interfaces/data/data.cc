// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "interfaces/data/data.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>

#include <boost/algorithm/string/join.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include "utils/database_operator.h"
#include "utils/date_time.h"
#include "utils/file.h"

using boost::filesystem::exists;
using boost::gregorian::day_clock;
using boost::join;
using boost::posix_time::to_iso_extended_string;
using boost::shared_lock;
using boost::shared_mutex;
using google::protobuf::TextFormat;
using std::endl;
using std::ios;
using std::ofstream;
using std::replace;
using std::string;
using std::to_string;
using std::unique_lock;
using std::vector;
using util::DatabaseConfig;
using util::DatabaseOperator;
using util::ReadFileLines;
using util::Within;

namespace interface {

Data::Data(const Security& security) : security_(security) {}

Data::~Data() {}

const Security& Data::GetSecurity() { return security_; }

void Data::InsertSnapshot(const Snapshot& snapshot) {
  unique_lock<shared_mutex> lock(snapshot_mutex_);

  Snapshot modified_snapshot = snapshot;

  if (snapshots_.empty()) {
    modified_snapshot.set_current_volume(snapshot.total_volume());
  } else {
    int64_t current_volume =
        snapshot.total_volume() - snapshots_.back().total_volume();
    modified_snapshot.set_current_volume(current_volume);
  }
  snapshots_.push_back(modified_snapshot);
}

Snapshot Data::GetLastSnapshot() {
  shared_lock<shared_mutex> lock(snapshot_mutex_);

  if (snapshots_.empty()) {
    return Snapshot();
  } else {
    return snapshots_.back();
  }
}

Snapshot Data::GetSnapshotAt(const uint32_t index) {
  shared_lock<shared_mutex> lock(snapshot_mutex_);

  if (snapshots_.size() <= index) {
    return Snapshot();
  } else {
    return snapshots_[index];
  }
}

Snapshot Data::GetLastSnapshotWithin(const string& timestamp,
                                     const uint32_t time_in_seconds) {
  shared_lock<shared_mutex> lock(snapshot_mutex_);

  for (auto iter = snapshots_.rbegin(); iter != snapshots_.rend(); iter++) {
    if (Within(iter->exchange_time(), timestamp, time_in_seconds)) {
      return *iter;
    }
  }

  return Snapshot();
}

vector<Snapshot> Data::GetSnapshots() { return snapshots_; }

vector<Snapshot> Data::GetLastSnapshots(const uint32_t number) {
  shared_lock<shared_mutex> lock(snapshot_mutex_);

  uint32_t size = snapshots_.size();

  if (size <= number) {
    return snapshots_;
  } else {
    vector<Snapshot> snapshots(snapshots_.begin() + size - number,
                               snapshots_.end());
    return snapshots;
  }
}

int32_t Data::SnapshotSize() { return snapshots_.size(); }

void Data::Dump(const string& data_dir, const string& session,
                const DatabaseConfig& config) {
  if (snapshots_.empty()) {
    LOG(WARNING) << "Empty snapshot for security <" << security_.id() << ">";

    return;
  }

  WriteToFile(data_dir, session);
  //WriteToDatabase(config);

  LOG(INFO) << "Dump <" << snapshots_.size() << "> snapshots for <"
            << security_.id() << ">";

  snapshots_.clear();
}

void Data::ReadFromFile(const std::string& file_name) {
  vector<string> lines = ReadFileLines(file_name);

  for (const auto& line : lines) {
    Snapshot snapshot;

    TextFormat::ParseFromString(line, &snapshot);

    snapshots_.push_back(snapshot);
  }
}

void Data::WriteToFile(const string& data_dir, const string& session) {
  string file_dir = data_dir + "/" + security_.id();
  
  string file_name =
      to_iso_extended_string(day_clock::local_day()) + "_" + session;

  if (!exists(file_dir)) {
    string command = "mkdir -p " + file_dir;
    
    LOG(INFO) << "Creating directory <" << file_dir
              << "> status <" << system(command.c_str()) << ">";
  }

  ofstream file(file_dir + "/" + file_name, ios::app);

	if (!file.is_open()) {
		LOG(ERROR) << "Fail to open file <" << file_dir + "/" + file_name << ">";
		
		return;
	}
	
  file << "instrument_id"
       << ","
       << "exchange_id"
       << ","
       << "exchange_inst_id"
       << ","
       << "last_price"
       << ","
       << "pre_settlement_price"
       << ","
       << "pre_close_price"
       << ","
       << "pre_open_interest"
       << ","
       << "open_price"
       << ","
       << "highest_price"
       << ","
       << "lowest_price"
       << ","
       << "total_volume"
       << ","
       << "current_volume"
       << ","
       << "turn_over"
       << ","
       << "open_interest"
       << ","
       << "close_price"
       << ","
       << "settlement_price"
       << ","
       << "upper_limit_price"
       << ","
       << "lower_limit_price"
       << ","
       << "pre_delta"
       << ","
       << "curr_delta"
       << ","
       << "level_1_bid_price"
       << ","
       << "level_2_bid_price"
       << ","
       << "level_3_bid_price"
       << ","
       << "level_4_bid_price"
       << ","
       << "level_5_bid_price"
       << ","
       << "level_1_bid_volume"
       << ","
       << "level_2_bid_volume"
       << ","
       << "level_3_bid_volume"
       << ","
       << "level_4_bid_volume"
       << ","
       << "level_5_bid_volume"
       << ","
       << "level_1_ask_price"
       << ","
       << "level_2_ask_price"
       << ","
       << "level_3_ask_price"
       << ","
       << "level_4_ask_price"
       << ","
       << "level_5_ask_price"
       << ","
       << "level_1_ask_volume"
       << ","
       << "level_2_ask_volume"
       << ","
       << "level_3_ask_volume"
       << ","
       << "level_4_ask_volume"
       << ","
       << "level_5_ask_volume"
       << ","
       << "average_price"
       << ","
       << "action_day"
       << ","
       << "trading_day"
       << ","
       << "exchange_time"
       << ","
       << "local_time" << endl;

  for (const auto& snapshot : snapshots_) {
    file << snapshot.instrument_id() << "," << snapshot.exchange_id() << ","
         << snapshot.exchange_inst_id() << "," << snapshot.last_price() << ","
         << snapshot.pre_settlement_price() << "," << snapshot.pre_close_price()
         << "," << snapshot.pre_open_interest() << "," << snapshot.open_price()
         << "," << snapshot.highest_price() << "," << snapshot.lowest_price()
         << "," << snapshot.total_volume() << "," << snapshot.current_volume()
         << "," << snapshot.turn_over() << "," << snapshot.open_interest()
         << "," << snapshot.close_price() << "," << snapshot.settlement_price()
         << "," << snapshot.upper_limit_price() << ","
         << snapshot.lower_limit_price() << "," << snapshot.pre_delta() << ","
         << snapshot.curr_delta() << "," << snapshot.level_1_bid_price() << ","
         << snapshot.level_2_bid_price() << "," << snapshot.level_3_bid_price()
         << "," << snapshot.level_4_bid_price() << ","
         << snapshot.level_5_bid_price() << "," << snapshot.level_1_bid_volume()
         << "," << snapshot.level_2_bid_volume() << ","
         << snapshot.level_3_bid_volume() << ","
         << snapshot.level_4_bid_volume() << ","
         << snapshot.level_5_bid_volume() << "," << snapshot.level_1_ask_price()
         << "," << snapshot.level_2_ask_price() << ","
         << snapshot.level_3_ask_price() << "," << snapshot.level_4_ask_price()
         << "," << snapshot.level_5_ask_price() << ","
         << snapshot.level_1_ask_volume() << ","
         << snapshot.level_2_ask_volume() << ","
         << snapshot.level_3_ask_volume() << ","
         << snapshot.level_4_ask_volume() << ","
         << snapshot.level_5_ask_volume() << "," << snapshot.average_price()
         << "," << snapshot.action_day() << "," << snapshot.trading_day() << ","
         << snapshot.exchange_time() << "," << snapshot.local_time() << endl;
  }
}

void Data::WriteToDatabase(const DatabaseConfig& config) {
  DatabaseOperator database_operator(config);

  if (!database_operator.Init()) {
    return;
  }

  string command = "INSERT INTO snapshots VALUES ";

  vector<string> records;

  for (const auto& snapshot : snapshots_) {
    string record =
        "('" + snapshot.instrument_id() + "','" + snapshot.exchange_id() +
        "','" + snapshot.exchange_inst_id() + "'," +
        to_string(snapshot.last_price()) + "," +
        to_string(snapshot.pre_settlement_price()) + "," +
        to_string(snapshot.pre_close_price()) + "," +
        to_string(snapshot.pre_open_interest()) + "," +
        to_string(snapshot.open_price()) + "," +
        to_string(snapshot.highest_price()) + "," +
        to_string(snapshot.lowest_price()) + "," +
        to_string(snapshot.total_volume()) + "," +
        to_string(snapshot.current_volume()) + "," +
        to_string(snapshot.turn_over()) + "," +
        to_string(snapshot.open_interest()) + "," +
        to_string(snapshot.close_price()) + "," +
        to_string(snapshot.settlement_price()) + "," +
        to_string(snapshot.upper_limit_price()) + "," +
        to_string(snapshot.lower_limit_price()) + "," +
        to_string(snapshot.pre_delta()) + "," +
        to_string(snapshot.curr_delta()) + "," +
        to_string(snapshot.level_1_bid_price()) + "," +
        to_string(snapshot.level_2_bid_price()) + "," +
        to_string(snapshot.level_3_bid_price()) + "," +
        to_string(snapshot.level_4_bid_price()) + "," +
        to_string(snapshot.level_5_bid_price()) + "," +
        to_string(snapshot.level_1_bid_volume()) + "," +
        to_string(snapshot.level_2_bid_volume()) + "," +
        to_string(snapshot.level_3_bid_volume()) + "," +
        to_string(snapshot.level_4_bid_volume()) + "," +
        to_string(snapshot.level_5_bid_volume()) + "," +
        to_string(snapshot.level_1_ask_price()) + "," +
        to_string(snapshot.level_2_ask_price()) + "," +
        to_string(snapshot.level_3_ask_price()) + "," +
        to_string(snapshot.level_4_ask_price()) + "," +
        to_string(snapshot.level_5_ask_price()) + "," +
        to_string(snapshot.level_1_ask_volume()) + "," +
        to_string(snapshot.level_2_ask_volume()) + "," +
        to_string(snapshot.level_3_ask_volume()) + "," +
        to_string(snapshot.level_4_ask_volume()) + "," +
        to_string(snapshot.level_5_ask_volume()) + "," +
        to_string(snapshot.average_price()) + ",'" + snapshot.action_day() +
        "','" + snapshot.trading_day() + "','" + snapshot.exchange_time() +
        "','" + snapshot.local_time() + "')";

    records.push_back(record);
  }

  command += join(records, ",") + ";";
  database_operator.Write(command);
}

}  // namespace
