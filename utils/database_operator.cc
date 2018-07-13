// Copyright 2016, Xuanting Cai
// Email:caixuanting@gmail.com

#include <glog/logging.h>

#include "utils/database_operator.h"

using sql::Connection;
using sql::Driver;
using sql::ResultSet;
using sql::Statement;
using std::exception;
using std::lock_guard;
using std::mutex;
using std::string;
using std::unique_ptr;

namespace util {

mutex DatabaseOperator::driver_mutex_;

DatabaseOperator::DatabaseOperator(const DatabaseConfig& config)
    : config_(config) {
  try {
    lock_guard<mutex> guard(driver_mutex_);
    driver_ = get_driver_instance();
    driver_->threadInit();
  } catch (const exception& e) {
    driver_ = nullptr;
    LOG(ERROR) << "Create database driver failed: " << e.what();
  }
}

DatabaseOperator::~DatabaseOperator() {
  if (driver_ != nullptr) {
    try {
      driver_->threadEnd();
    } catch (const exception& e) {
      LOG(ERROR) << "Destroy database driver failed: " << e.what();
    }
  }
}

bool DatabaseOperator::Init() {
  if (driver_ == nullptr) {
    LOG(ERROR) << "driver_ is nullptr";
    return false;
  }

  try {
    connection_.reset(
        driver_->connect(config_.host(), config_.user(), config_.password()));
  } catch (const exception& e) {
    LOG(ERROR) << "Create connection failed: " << e.what();
    return false;
  }

  if (connection_->isClosed()) {
    LOG(ERROR) << "connection_ is closed";
    return false;
  }

  try {
    connection_->setSchema(config_.database());
  } catch (const exception& e) {
    LOG(ERROR) << "Select database <" << config_.database()
               << "> failed: " << e.what();
    return false;
  }

  return true;
}

Driver* DatabaseOperator::GetDriver() { return driver_; }

Connection* DatabaseOperator::GetConnection() { return connection_.get(); }

ResultSet* DatabaseOperator::Read(const string& command) {
  ResultSet* result_set = nullptr;

  if (connection_ == nullptr) {
    LOG(ERROR) << "Connection is nullptr";
    return result_set;
  }

  try {
    unique_ptr<Statement> statement(connection_->createStatement());
    result_set = statement->executeQuery(command);
  } catch (const exception& e) {
    LOG(ERROR) << "Execute command <" << command << "> failed: " << e.what();
  }

  return result_set;
}

void DatabaseOperator::Write(const string& command) {
  if (connection_ == nullptr) {
    LOG(ERROR) << "Connection is nullptr";
    return;
  }

  try {
    unique_ptr<Statement> statement(connection_->createStatement());
    statement->execute(command);
    connection_->commit();
  } catch (const exception& e) {
    LOG(ERROR) << "Execute command <" << command << "> failed: " << e.what();
  }
}

}  // namespace util
