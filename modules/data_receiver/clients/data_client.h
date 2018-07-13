// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef MODULES_DATA_RECEIVER_CLIENTS_DATA_CLIENT_H_
#define MODULES_DATA_RECEIVER_CLIENTS_DATA_CLIENT_H_

#include "interfaces/data/data_pool.h"

namespace data_receiver {

class DataClient {
 public:
  DataClient(interface::DataPool* data_pool);
  virtual ~DataClient();

  virtual void Init() = 0;
  virtual void Start() = 0;
  virtual void Exit() = 0;

 protected:
  interface::DataPool* data_pool_;  // Not owned.
};

}  // namespace data_receiver

#endif  // MODULES_DATA_RECEIVER_CLINETS_DATA_CLIENT_H_
