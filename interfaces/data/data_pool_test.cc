// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "interfaces/data/data_pool.h"

#include <boost/test/unit_test.hpp>

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

using google::protobuf::TextFormat;
using util::DatabaseConfig;

namespace interface {

struct DataPoolFixture {
  DataPoolFixture() {
    Security security;
    TextFormat::ParseFromString(
        R"(country: "test_country")"
        R"(exchange: "test_exchange")"
        R"(id : "test_id")",
        &security);

    Snapshot snapshot;
    TextFormat::ParseFromString(R"(exchange_id: "test_exchange")"
                                R"(instrument_id: "test_instrument_id")",
                                &snapshot);

    Data* data = new Data(security);

    SecurityPool security_pool;
    data_pool.reset(new DataPool(security_pool));

    data_pool->InsertData(security.id(), data);
    data_pool->InsertSnapshot(security.id(), snapshot);
    // Insert failure.
    data_pool->InsertSnapshot("non-exist-id", snapshot);
  }

  virtual ~DataPoolFixture() {}

  std::unique_ptr<DataPool> data_pool;
};

BOOST_FIXTURE_TEST_SUITE(DataPoolTests, DataPoolFixture)

BOOST_AUTO_TEST_CASE(InitFromDatabase) {
  DatabaseConfig config;

  TextFormat::ParseFromString(
      R"(host: "localhost")"
      R"(user: "root")"
      R"(password: "cai1984")"
      R"(database: "test")",
      &config);

  DataPool data_pool(config);

  BOOST_CHECK_EQUAL(1, data_pool.Size());
}

BOOST_AUTO_TEST_CASE(IterateData) {
  BOOST_CHECK_EQUAL(1, data_pool->Size());

  IdToData::iterator begin = data_pool->begin();

  const Security& security = begin->second->GetSecurity();
  BOOST_CHECK_EQUAL("test_country", security.country());
  BOOST_CHECK_EQUAL("test_exchange", security.exchange());
  BOOST_CHECK_EQUAL("test_id", security.id());

  Snapshot snapshot = begin->second->GetLastSnapshot();
  BOOST_CHECK_EQUAL("test_exchange", snapshot.exchange_id());
  BOOST_CHECK_EQUAL("test_instrument_id", snapshot.instrument_id());
}

BOOST_AUTO_TEST_CASE(GetDataById) {
  Data* data = data_pool->GetDataById("non-exist-id");
  BOOST_CHECK(nullptr == data);

  data = data_pool->GetDataById("test_id");
  BOOST_CHECK(data != nullptr);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace interface
