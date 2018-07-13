// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include "http_server/http_server.h"
#include "modules/data_receiver/data_receiver.h"
#include "modules/event_scheduler/event_scheduler.h"
#include "modules/signal_manager/signal_manager.h"
#include "proto/system_config.pb.h"
#include "utils/file.h"

using data_receiver::DataMessage;
using data_receiver::DataReceiver;
using data_receiver::DataReceiverConfig;
using event_scheduler::Event;
using event_scheduler::EventScheduler;
using event_scheduler::EventSchedulerConfig;
using google::InitGoogleLogging;
using google::ParseCommandLineFlags;
using google::protobuf::TextFormat;
using http_server::HttpServer;
using interface::DataPool;
using interface::SignalPool;
using signal_manager::SignalManager;
using signal_manager::SignalManagerConfig;
using signal_manager::SignalMessage;
using std::cin;
using std::string;
using std::thread;
using std::unique_ptr;
using util::DatabaseConfig;
using util::Queue;
using util::ReadFile;

DEFINE_string(signal_recorder_config_file, "signal_recorder_config",
              "Configuration file for signal recorder.");

int main(int argc, char** argv) {
  InitGoogleLogging(argv[0]);
  ParseCommandLineFlags(&argc, &argv, true);

  string system_config_string = ReadFile(FLAGS_signal_recorder_config_file);

  SystemConfig system_config;
  TextFormat::ParseFromString(system_config_string, &system_config);

  // =====================================
  // ========== Event Scheduler ==========
  // =====================================

  // Create event queue.
  unique_ptr<Queue<Event*>> event_queue(new Queue<Event*>());

  // Create event scheduler.
  EventScheduler event_scheduler(system_config.event_scheduler_config(),
                                 event_queue.get());

  event_scheduler.Init();
  event_scheduler.Start();

  // =====================================
  // ========== Event Scheduler ==========
  // =====================================

  std::this_thread::sleep_for(std::chrono::seconds(1));

  // ===================================
  // ========== Data Receiver ==========
  // ===================================

  // Create data pool.
  unique_ptr<DataPool> data_pool(new DataPool(system_config.database_config()));

  // Create data message queue.
  unique_ptr<Queue<DataMessage>> data_message_queue(new Queue<DataMessage>());

  // Create data receiver.
  DataReceiver data_receiver(system_config.data_receiver_config(),
                             data_pool.get(), event_queue.get(),
                             data_message_queue.get());

  data_receiver.Init();
  data_receiver.Start();

  // ===================================
  // ========== Data Receiver ==========
  // ===================================

  // ====================================
  // ========== Signal Manager ==========
  // ====================================

  // Create signal pool.
  unique_ptr<SignalPool> signal_pool(
      new SignalPool(system_config.signal_configs(), data_pool.get()));

  // Create signal message queue.
  unique_ptr<Queue<SignalMessage>> signal_message_queue(
      new Queue<SignalMessage>());

  SignalManager signal_manager(
      system_config.signal_manager_config(), signal_pool.get(),
      event_queue.get(), data_message_queue.get(), signal_message_queue.get());

  signal_manager.Init();
  signal_manager.Start();

  // ====================================
  // ========== Data Receiver ===========
  // ====================================

  HttpServer http_server(system_config.http_server_config(),
                         event_scheduler.GetDescriptionToEvent(),
                         data_pool.get(), nullptr /* signal_pool */);

  http_server.Init();
  http_server.Start();

  thread signal_message_thread([&http_server, &signal_message_queue] {
    while (http_server.Running()) {
      signal_message_queue->Wait();

      signal_message_queue->Next();
    }
  });

  while (http_server.Running()) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  signal_message_queue->Notify();

  signal_message_thread.join();

  http_server.Exit();
  signal_manager.Exit();
  data_receiver.Exit();
  event_scheduler.Exit();

  return 0;
}
