// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef INTERFACES_DATA_DATA_H_
#define INTERFACES_DATA_DATA_H_

#include <string>
#include <vector>

#include <boost/thread.hpp>

#include "interfaces/data/proto/security.pb.h"
#include "interfaces/data/proto/snapshot.pb.h"
#include "utils/proto/database_config.pb.h"

namespace interface {

class Data {
 public:
  Data(const Security& security);
  virtual ~Data();

  const Security& GetSecurity();

  void InsertSnapshot(const Snapshot& snapshot);

  // Return default snapshot if there is no snapshot.
  Snapshot GetLastSnapshot();
  // Return default snapshot if there is no snapshot.
  Snapshot GetSnapshotAt(const uint32_t index);
  // Get last snapshot inside [timestamp - time_in_seconds, timestamp).
  // Return default snapshot if there is no snapshot.
  Snapshot GetLastSnapshotWithin(const std::string& timestamp,
                                 const uint32_t time_in_seconds);
  // Return all snapshots.
  std::vector<Snapshot> GetSnapshots();
  // Return most recent N snapshots.
  std::vector<Snapshot> GetLastSnapshots(const uint32_t number);

  int32_t SnapshotSize();

  void Dump(const std::string& data_dir, const std::string& session,
            const util::DatabaseConfig& config);

  void ReadFromFile(const std::string& file_name);

 private:
  void WriteToFile(const std::string& data_dir, const std::string& session);
  void WriteToDatabase(const util::DatabaseConfig& config);

  Security security_;

  boost::shared_mutex snapshot_mutex_;
  std::vector<Snapshot> snapshots_;
};

}  // namespace inteface

#endif  // INTERFACES_DATA_H_
