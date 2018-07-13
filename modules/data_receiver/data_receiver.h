// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com
//
// This is the module receiving data from exchanges.
// Tasks:
//   1 Schedule data subscription
//   2 Schedule data dump
//   3 Notify signal module

#ifndef MODULES_DATA_RECEIVER_DATA_RECEIVER_H_
#define MODULES_DATA_RECEIVER_DATA_RECEIVER_H_

#include "modules/data_receiver/clients/ctp_future_data_client.h"
#include "modules/data_receiver/proto/data_receiver_config.pb.h"

namespace data_receiver {

typedef std::unordered_map<std::string, std::unique_ptr<DataClient>>
    NameToDataClient;

class DataReceiver {
 public:
  DataReceiver(const DataReceiverConfig& config, interface::DataPool* data_pool,
               util::Queue<event_scheduler::Event*>* event_queue,
               util::Queue<DataMessage>* data_message_queue);
  virtual ~DataReceiver();

  // Schedule data dump events.
  void Init();
  
  // Start clients.
  void Start();
  
  // Stop clients.
  void Exit();

 private:
  DataReceiverConfig config_;
  NameToDataClient name_to_data_client_;
  boost::thread_group client_threads_;
  interface::DataPool* data_pool_;  // Not owned.
  std::vector<std::unique_ptr<event_scheduler::Event>> events_;
  util::Queue<event_scheduler::Event*>* event_queue_;  // Not owned.
};

}  // namespace data_receiver

#endif  // MODULES_DATA_RECEIVER_DATA_RECEIVER_H_