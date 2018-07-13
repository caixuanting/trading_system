// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "utils/database_operator.h"

#include <vector>

#include <boost/test/unit_test.hpp>

#include <google/protobuf/text_format.h>

using google::protobuf::TextFormat;
using sql::ResultSet;
using std::unique_ptr;
using std::vector;

namespace util {

struct DatabaseOperatorFixture {
  DatabaseOperatorFixture() {
    DatabaseConfig database_config;

    TextFormat::ParseFromString(
        R"(host: "localhost")"
        R"(user: "root")"
        R"(password: "cai1984")"
        R"(database: "test")",
        &database_config);

    database_operator.reset(new DatabaseOperator(database_config));
  }

  virtual ~DatabaseOperatorFixture() {}

  unique_ptr<DatabaseOperator> database_operator;
};

BOOST_FIXTURE_TEST_SUITE(DatabaseOperatorTest, DatabaseOperatorFixture)

BOOST_AUTO_TEST_CASE(GetDriver) {
  BOOST_CHECK(database_operator->GetDriver() != nullptr);
}

BOOST_AUTO_TEST_CASE(Connection) {
  BOOST_CHECK(database_operator->Init());
  BOOST_CHECK(database_operator->GetConnection() != nullptr);

  database_operator->Write(
      "INSERT INTO numbers VALUES (1), (2), (3), (4), (5);");

  unique_ptr<ResultSet> result_set(
      database_operator->Read("SELECT * FROM numbers LIMIT 3;"));

  BOOST_CHECK(result_set != nullptr);

  vector<int32_t> numbers;

  while (result_set->next()) {
    numbers.push_back(result_set->getInt("number"));
  }

  BOOST_CHECK_EQUAL(3, numbers.size());
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace util
