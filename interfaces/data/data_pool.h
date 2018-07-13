// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef INTERFACES_DATA_DATA_POOL_H_
#define INTERFACES_DATA_DATA_POOL_H_

#include <string>
#include <unordered_map>

#include "interfaces/data/data.h"
#include "interfaces/data/proto/security.pb.h"
#include "utils/proto/database_config.pb.h"

namespace interface {
	
typedef std::unordered_map<std::string, std::unique_ptr<Data>> IdToData;

class DataPool {
 public:
  DataPool(const util::DatabaseConfig& config);
  DataPool(const SecurityPool& security_pool);
  
  virtual ~DataPool();

  IdToData::iterator begin();
  IdToData::iterator end();

  size_t Size();

  void InsertData(const std::string& id, Data* data);

  Data* GetDataById(const std::string& id);

  void InsertSnapshot(const std::string& id, const Snapshot& snapshot);

 private:
  IdToData id_to_data_;
};

}  // namespace interface

#endif  // INTERFACES_DATA_DATA_POOL_H_
