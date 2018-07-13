// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef UTILS_DATABASE_OPERATOR_H_
#define UTILS_DATABASE_OPERATOR_H_

#include <exception>
#include <mutex>
#include <string>

#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "utils/proto/database_config.pb.h"

namespace util {

class DatabaseOperator {
 public:
  DatabaseOperator(const DatabaseConfig& config);
  virtual ~DatabaseOperator();

  bool Init();

  sql::Driver* GetDriver();
  sql::Connection* GetConnection();

  sql::ResultSet* Read(const std::string& command);
  void Write(const std::string& command);

 protected:
  sql::Driver* driver_;  // Not owned.
  static std::mutex driver_mutex_;
  DatabaseConfig config_;
  std::string database_;
  std::string host_;
  std::string password_;
  std::string user_;
  std::unique_ptr<sql::Connection> connection_;
};

}  // namespace util

#endif  // UTILS_DATABASE_OPERATOR_H_
