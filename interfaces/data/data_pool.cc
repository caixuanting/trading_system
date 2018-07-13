// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "interfaces/data/data_pool.h"

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include "utils/database_operator.h"

using google::protobuf::TextFormat;
using sql::ResultSet;
using std::move;
using std::string;
using std::unique_ptr;
using util::DatabaseConfig;
using util::DatabaseOperator;
using util::TimeInterval;

namespace interface {

DataPool::DataPool(const DatabaseConfig& config) {
  DatabaseOperator database_operator(config);

  if (!database_operator.Init()) {
    return;
  }

  unique_ptr<ResultSet> result_set(
      database_operator.Read("SELECT * FROM securities;"));

  if (result_set != nullptr) {
    while (result_set->next()) {
      Security security;

      TextFormat::ParseFromString(result_set->getString("security"), &security);

      string command = "SELECT trading_time FROM trading_times WHERE ";
      command += "country='" + security.country() + "'";
      command += " AND exchange='" + security.exchange() + "'";
      command += " AND product='" + security.product() + "';";

      unique_ptr<ResultSet> trading_time_result_set(
          database_operator.Read(command));

      if (trading_time_result_set != nullptr) {
        while (trading_time_result_set->next()) {
          TimeInterval time_interval;

          TextFormat::ParseFromString(
              trading_time_result_set->getString("trading_time"),
              &time_interval);
          *security.add_time_interval() = time_interval;
        }
      }

      if (id_to_data_.count(security.id()) == 0) {
        id_to_data_[security.id()].reset(new Data(security));
      } else {
        LOG(INFO) << "Security <" << security.id() << "> already exist";
      }
    }
  }
}

DataPool::DataPool(const SecurityPool& security_pool) {
  for (const auto& security : security_pool.security()) {
    Data* data = new Data(security);

    if (id_to_data_.count(security.id()) == 0) {
      id_to_data_[security.id()].reset(data);
    } else {
      LOG(INFO) << "Security <" << security.id() << "> already exist";
    }
  }
}

DataPool::~DataPool() {}

IdToData::iterator DataPool::begin() { return id_to_data_.begin(); }

IdToData::iterator DataPool::end() { return id_to_data_.end(); }

size_t DataPool::Size() { return id_to_data_.size(); }

void DataPool::InsertData(const string& id, Data* data) {
  id_to_data_[id].reset(data);
}

Data* DataPool::GetDataById(const string& id) {
  if (id_to_data_.count(id) == 0) {
    LOG(WARNING) << "Data " << id << " not found";
    return nullptr;
  } else {
    return id_to_data_[id].get();
  }
}

void DataPool::InsertSnapshot(const string& id, const Snapshot& snapshot) {
  if (id_to_data_.count(id) != 0) {
    id_to_data_[id]->InsertSnapshot(snapshot);
  } else {
    LOG(WARNING) << "Security " << id << " not found";
  }
}

}  // namespace interface
