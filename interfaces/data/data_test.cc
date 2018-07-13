// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "interfaces/data/data.h"

#include <boost/test/unit_test.hpp>

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include "utils/date_time.h"

using google::protobuf::TextFormat;
using std::vector;
using util::DatabaseConfig;

namespace interface {

struct DataFixture {
  DataFixture() {
    Security security;
    TextFormat::ParseFromString(
        R"(country: "test_country")"
        R"(exchange: "test_exchange")"
        R"(id: "test_id")",
        &security);

    data.reset(new Data(security));

    {
      Snapshot snapshot;

      TextFormat::ParseFromString(
          R"(exchange_id: "test_exchange")"
          R"(instrument_id: "test_instrument_id")"
          R"(action_day: "2016-12-31")"
          R"(exchange_time: "2016-12-31 10:00:00")"
          R"(local_time: "2016-12-31 10:00:00")"
          R"(total_volume: 10)"
          R"(is_valid: true)",
          &snapshot);

      data->InsertSnapshot(snapshot);
    }

    {
      Snapshot snapshot;

      TextFormat::ParseFromString(
          R"(exchange_id: "test_exchange")"
          R"(instrument_id: "test_instrument_id")"
          R"(action_day: "2016-12-31")"
          R"(exchange_time: "2016-12-31 10:00:01")"
          R"(local_time: "2016-12-31 10:00:01")"
          R"(total_volume: 11)"
          R"(is_valid: true)",
          &snapshot);

      data->InsertSnapshot(snapshot);
    }
  }

  virtual ~DataFixture() {}

  std::unique_ptr<Data> data;
};

BOOST_FIXTURE_TEST_SUITE(DataTest, DataFixture)

BOOST_AUTO_TEST_CASE(GetSecurityTest) {
  const Security& security = data->GetSecurity();

  BOOST_CHECK_EQUAL("test_country", security.country());
  BOOST_CHECK_EQUAL("test_exchange", security.exchange());
  BOOST_CHECK_EQUAL("test_id", security.id());
}

BOOST_AUTO_TEST_CASE(GetLastSnapshotTest) {
  Snapshot snapshot = data->GetLastSnapshot();

  BOOST_CHECK_EQUAL("test_exchange", snapshot.exchange_id());
  BOOST_CHECK_EQUAL("test_instrument_id", snapshot.instrument_id());
  BOOST_CHECK(snapshot.is_valid());
  BOOST_CHECK_EQUAL(11, snapshot.total_volume());
  BOOST_CHECK_EQUAL(1, snapshot.current_volume());
}

BOOST_AUTO_TEST_CASE(GetSnapshotAtTest) {
  {
    Snapshot snapshot = data->GetSnapshotAt(0);

    BOOST_CHECK_EQUAL("test_exchange", snapshot.exchange_id());
    BOOST_CHECK_EQUAL("test_instrument_id", snapshot.instrument_id());
    BOOST_CHECK(snapshot.is_valid());
    BOOST_CHECK_EQUAL(10, snapshot.total_volume());
    BOOST_CHECK_EQUAL(10, snapshot.current_volume());
  }
  {
    Snapshot snapshot = data->GetSnapshotAt(1);

    BOOST_CHECK_EQUAL("test_exchange", snapshot.exchange_id());
    BOOST_CHECK_EQUAL("test_instrument_id", snapshot.instrument_id());
    BOOST_CHECK(snapshot.is_valid());
    BOOST_CHECK_EQUAL(11, snapshot.total_volume());
    BOOST_CHECK_EQUAL(1, snapshot.current_volume());
  }
  {
    Snapshot snapshot = data->GetSnapshotAt(2);

    BOOST_CHECK_EQUAL(false, snapshot.is_valid());
  }
}

BOOST_AUTO_TEST_CASE(GetLastSnapshotWithinTest) {
  {
    Snapshot snapshot = data->GetLastSnapshotWithin("2016-12-31 10:00:00", 1);

    BOOST_CHECK(!snapshot.is_valid());
  }
  {
    Snapshot snapshot = data->GetLastSnapshotWithin("2016-12-31 10:00:01", 1);

    BOOST_CHECK_EQUAL("test_exchange", snapshot.exchange_id());
    BOOST_CHECK_EQUAL("test_instrument_id", snapshot.instrument_id());
    BOOST_CHECK(snapshot.is_valid());
    BOOST_CHECK_EQUAL(10, snapshot.total_volume());
    BOOST_CHECK_EQUAL(10, snapshot.current_volume());
  }
}

BOOST_AUTO_TEST_CASE(GetSnapshotsTest) {
  vector<Snapshot> snapshots = data->GetSnapshots();

  BOOST_CHECK_EQUAL(2, snapshots.size());
}

BOOST_AUTO_TEST_CASE(GetLastSnapshotsTest) {
  {
    vector<Snapshot> snapshots = data->GetLastSnapshots(1);

    BOOST_CHECK_EQUAL(1, snapshots.size());

    BOOST_CHECK_EQUAL("test_exchange", snapshots[0].exchange_id());
    BOOST_CHECK_EQUAL("test_instrument_id", snapshots[0].instrument_id());
    BOOST_CHECK(snapshots[0].is_valid());
    BOOST_CHECK_EQUAL(11, snapshots[0].total_volume());
    BOOST_CHECK_EQUAL(1, snapshots[0].current_volume());
  }
  {
    vector<Snapshot> snapshots = data->GetLastSnapshots(2);

    BOOST_CHECK_EQUAL(2, snapshots.size());
  }
}

BOOST_AUTO_TEST_CASE(DumpTest) {
  DatabaseConfig config;

  TextFormat::ParseFromString(
      R"(host: "localhost")"
      R"(user: "root")"
      R"(password: "cai1984")"
      R"(database: "test")",
      &config);

  data->Dump("test", "test_session", config);

  Snapshot snapshot = data->GetLastSnapshot();

  BOOST_CHECK_EQUAL(false, snapshot.is_valid());
}

BOOST_AUTO_TEST_CASE(ReadFromFileTest) {
  Security security;
  TextFormat::ParseFromString(
      R"(country: "test_country")"
      R"(exchange: "test_exchange")"
      R"(id: "test_id")",
      &security);

  Data data(security);
  data.ReadFromFile("test/data/data");

  LOG(INFO) << data.GetLastSnapshot().DebugString();

  BOOST_CHECK_EQUAL(7525, data.SnapshotSize());
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace interface
