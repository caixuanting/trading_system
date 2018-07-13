// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include "http_server/http_server.h"
#include "modules/account_keeper/account_keeper.h"
#include "modules/data_receiver/data_receiver.h"
#include "modules/event_scheduler/event_scheduler.h"
#include "modules/order_maker/order_maker.h"
#include "modules/signal_manager/signal_manager.h"
#include "modules/strategy_runner/strategy_runner.h"
#include "proto/system_config.pb.h"
#include "utils/file.h"

using account_keeper::AccountKeeper;
using account_keeper::AccountMessage;
using data_receiver::DataMessage;
using data_receiver::DataReceiver;
using event_scheduler::Event;
using event_scheduler::EventScheduler;
using google::InitGoogleLogging;
using google::ParseCommandLineFlags;
using google::protobuf::TextFormat;
using http_server::HttpServer;
using interface::AccountPool;
using interface::DataPool;
using interface::OrderFactory;
using interface::SignalPool;
using order_maker::OrderMaker;
using order_maker::OrderMessage;
using signal_manager::SignalManager;
using signal_manager::SignalMessage;
using std::string;
using std::unique_ptr;
using strategy_runner::StrategyMessage;
using strategy_runner::StrategyRunner;
using util::Queue;
using util::ReadFile;

DEFINE_string(system_config_file, "system_config",
              "Configuration file for the system.");

int main(int argc, char** argv) {
  InitGoogleLogging(argv[0]);
  ParseCommandLineFlags(&argc, &argv, true);

  string system_config_string = ReadFile(FLAGS_system_config_file);

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

  // ====================================
  // ========== Account Keeper ==========
  // ====================================

  // Create account pool.
  unique_ptr<AccountPool> account_pool(new AccountPool());

  // Create account message queue.
  unique_ptr<Queue<AccountMessage>> account_message_queue(
      new Queue<AccountMessage>());

  // Create account keeper.
  AccountKeeper account_keeper(system_config.account_keeper_config(),
                               account_pool.get(), account_message_queue.get());

  account_keeper.Init();
  account_keeper.Start();

  // ====================================
  // ========== Account Keeper ==========
  // ====================================

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

  std::this_thread::sleep_for(std::chrono::seconds(3));

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
  // ========== Signal Manager ==========
  // ====================================

  std::this_thread::sleep_for(std::chrono::seconds(1));

  // =================================
  // ========== Order Maker ==========
  // =================================

  // Create strategy message queue.
  unique_ptr<Queue<StrategyMessage>> strategy_message_queue(
      new Queue<StrategyMessage>());

  // Create order message queue.
  unique_ptr<Queue<OrderMessage>> order_message_queue(
      new Queue<OrderMessage>());

  OrderMaker order_maker(system_config.order_maker_config(), account_pool.get(),
                         order_message_queue.get(),
                         strategy_message_queue.get(), event_queue.get());

  order_maker.Init();
  order_maker.Start();

  // =================================
  // ========== Order Maker ==========
  // =================================

  std::this_thread::sleep_for(std::chrono::seconds(3));

  // =====================================
  // ========== Strategy Runner ==========
  // =====================================

  // Create order factory.
  unique_ptr<OrderFactory> order_factory(new OrderFactory());

  StrategyRunner strategy_runner(
      system_config.strategy_runner_config(), order_factory.get(),
      account_pool.get(), data_pool.get(), signal_pool.get(), event_queue.get(),
      strategy_message_queue.get(), signal_message_queue.get(),
      order_message_queue.get());

  strategy_runner.Init();
  strategy_runner.Start();

  // =====================================
  // ========== Strategy Runner ==========
  // =====================================

  std::this_thread::sleep_for(std::chrono::seconds(3));

  // =================================
  // ========== Http Server ==========
  // =================================

  HttpServer http_server(system_config.http_server_config(),
                         event_scheduler.GetDescriptionToEvent(),
                         data_pool.get(), signal_pool.get());

  http_server.Init();
  http_server.Start();

  // =================================
  // ========== Http Server ==========
  // =================================

  std::this_thread::sleep_for(std::chrono::seconds(3));

  string close;

  while (close != "y") {
    std::cin >> close;
  }

  http_server.Exit();
  order_maker.Exit();
  strategy_runner.Exit();
  event_scheduler.Exit();
  signal_manager.Exit();
  data_receiver.Exit();
  account_keeper.Exit();

  return 0;
}
