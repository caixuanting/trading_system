// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include "modules/event_scheduler/event_scheduler.h"
#include "modules/order_maker/order_maker.h"
#include "proto/system_config.pb.h"
#include "utils/database_operator.h"
#include "utils/file.h"

using event_scheduler::Event;
using event_scheduler::EventScheduler;
using google::InitGoogleLogging;
using google::ParseCommandLineFlags;
using google::protobuf::TextFormat;
using interface::AccountPool;
using order_maker::OrderMaker;
using order_maker::OrderMessage;
using std::string;
using std::unique_ptr;
using strategy_runner::StrategyMessage;
using util::DatabaseOperator;
using util::Queue;
using util::ReadFile;

DEFINE_string(update_security_config_file, "update_security_config",
              "Configuration file for update security.");

int main(int argc, char** argv) {
  InitGoogleLogging(argv[0]);
  ParseCommandLineFlags(&argc, &argv, true);

  string system_config_string = ReadFile(FLAGS_update_security_config_file);

  SystemConfig system_config;
  TextFormat::ParseFromString(system_config_string, &system_config);

  // Create event queue.
  unique_ptr<Queue<Event*>> event_queue(new Queue<Event*>());

  // Create event scheduler.
  EventScheduler event_scheduler(system_config.event_scheduler_config(),
                                 event_queue.get());

  event_scheduler.Init();
  event_scheduler.Start();

  // Create account pool.
  unique_ptr<AccountPool> account_pool(new AccountPool());

  // Create strategy message queue.
  unique_ptr<Queue<StrategyMessage>> strategy_message_queue(
      new Queue<StrategyMessage>());

  // Create order message queue.
  unique_ptr<Queue<OrderMessage>> order_message_queue(
      new Queue<OrderMessage>());

  DatabaseOperator database_operator(system_config.database_config());

  if (!database_operator.Init()) {
    LOG(ERROR) << "Fail to init database operator";

    return 1;
  }

  database_operator.Write("delete from securities;");

  OrderMaker order_maker(system_config.order_maker_config(), account_pool.get(),
                         order_message_queue.get(),
                         strategy_message_queue.get(), event_queue.get());

  order_maker.Init();
  order_maker.Start();

  std::this_thread::sleep_for(std::chrono::seconds(5));

  string exchange;

  while (exchange != "exit") {
    std::cout << "Which exchange: ";
    std::cin >> exchange;

    if (exchange == "CFFEX" || exchange == "SHFE" || exchange == "DCE" ||
        exchange == "CZCE" || exchange == "*") {
      OrderMessage order_message;
      TextFormat::ParseFromString(
          R"(client_name: "YIN_HE_CTP_FUTURE")"
          R"(command: REQUEST_SECURITY_DETAIL)",
          &order_message);
      if (exchange != "*") {
        order_message.set_exchange(exchange);
      }

      order_message_queue->Insert(order_message);
      order_message_queue->Notify();

      std::this_thread::sleep_for(std::chrono::seconds(20));
    }
  }

  event_scheduler.Exit();
  order_maker.Exit();

  return 0;
}
