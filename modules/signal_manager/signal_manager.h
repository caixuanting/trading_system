// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com
//
// This is the module managing all signals.
// Tasks:
//   1 Schedule signal lookup.
//   2 Schedule signal dump.
//   3 Notify strategies.

#ifndef MODULES_SIGNAL_MANAGER_SIGNAL_MANAGER_H_
#define MODULES_SIGNAL_MANAGER_SIGNAL_MANAGER_H_

#include <thread>

#include "interfaces/signal/signal_pool.h"
#include "modules/data_receiver/proto/data_message.pb.h"
#include "modules/event_scheduler/event.h"
#include "modules/signal_manager/proto/signal_manager_config.pb.h"
#include "modules/signal_manager/proto/signal_message.pb.h"
#include "utils/queue.h"

namespace signal_manager {

typedef std::unordered_map<std::string, std::string> DataIdToSignalName;

class SignalManager {
 public:
  SignalManager(const SignalManagerConfig& config,
                interface::SignalPool* signal_pool,
                util::Queue<event_scheduler::Event*>* event_queue,
                util::Queue<data_receiver::DataMessage>* data_message_queue,
                util::Queue<SignalMessage>* signal_message_queue);
  virtual ~SignalManager();

  // Schedule "look up" and "dump" events.
  void Init();

  // Run message processing thread.
  void Start();

  // Cancel events.
  // Stop message processing thread.
  void Exit();

 private:
  DataIdToSignalName data_id_to_signal_name_;
  SignalManagerConfig config_;
  bool running_ = true;
  interface::SignalPool* signal_pool_;  // Not owned.
  std::thread message_thread_;
  std::vector<std::unique_ptr<event_scheduler::Event>> events_;
  util::Queue<SignalMessage>* signal_message_queue_;             // Not owned.
  util::Queue<data_receiver::DataMessage>* data_message_queue_;  // Not owned.
  util::Queue<event_scheduler::Event*>* event_queue_;            // Not owned.
};

}  // namespace signal_manager

#endif  // MODULES_SIGNAL_MANGER_SIGNAL_MANAGER_H_