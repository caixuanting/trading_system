// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "modules/data_receiver/clients/data_client.h"

using interface::DataPool;

namespace data_receiver {

DataClient::DataClient(DataPool* data_pool) : data_pool_(data_pool) {}

DataClient::~DataClient() {}

}  // namespace data_receiver
