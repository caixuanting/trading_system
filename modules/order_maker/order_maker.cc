// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include <glog/logging.h>

#include "modules/order_maker/order_maker.h"

using event_scheduler::Event;
using interface::AccountPool;
using std::thread;
using std::string;
using strategy_runner::StrategyMessage;
using util::Queue;

namespace order_maker {

OrderMaker::OrderMaker(const OrderMakerConfig& config,
                       AccountPool* account_pool,
                       Queue<OrderMessage>* order_message_queue,
                       Queue<StrategyMessage>* strategy_message_queue,
                       Queue<Event*>* event_queue)
    : config_(config),
      account_pool_(account_pool),
      order_message_queue_(order_message_queue),
      event_queue_(event_queue),
      strategy_message_queue_(strategy_message_queue) {
  for (const auto& client_config : config.ctp_future_order_client_config()) {
    name_to_order_message_queue_[client_config.name()].reset(
        new Queue<OrderMessage>());

    name_to_order_client_[client_config.name()].reset(new CtpFutureOrderClient(
        client_config, account_pool, event_queue,
        name_to_order_message_queue_[client_config.name()].get(),
        strategy_message_queue));
  }
}

OrderMaker::~OrderMaker() {}

void OrderMaker::Init() {
  LOG(INFO) << "Initing order maker";

  for (auto iter = name_to_order_client_.begin();
       iter != name_to_order_client_.end(); iter++) {
    iter->second->Init();
  }

  LOG(INFO) << "Inited order maker";
}

void OrderMaker::Start() {
  LOG(INFO) << "Starting order maker";

  for (auto iter = name_to_order_client_.begin();
       iter != name_to_order_client_.end(); iter++) {
    client_threads_.push_back(thread([iter] { iter->second->Start(); }));
  }

  order_message_thread_ = thread([this] {
    while (running_) {
      this->order_message_queue_->Wait();

      if (this->running_) {
        while (!this->order_message_queue_->Empty()) {
          OrderMessage order_message = this->order_message_queue_->Next();
          LOG(INFO) << order_message.DebugString();

          string client_name = order_message.client_name();

          if (this->name_to_order_message_queue_.count(client_name) == 0) {
            LOG(WARNING) << "Client <" << client_name << "> doest not exist";

            continue;
          }

          this->name_to_order_message_queue_[client_name]->Insert(
              order_message);
          this->name_to_order_message_queue_[client_name]->Notify();
        }
      }
    }
  });

  LOG(INFO) << "Started order maker";
}

void OrderMaker::Exit() {
  LOG(INFO) << "Exiting order maker";

  running_ = false;
  order_message_queue_->Notify();
  order_message_thread_.join();

  for (auto iter = name_to_order_client_.begin();
       iter != name_to_order_client_.end(); iter++) {
    iter->second->Exit();
  }

  for (auto& client_thread : client_threads_) {
    client_thread.join();
  }

  LOG(INFO) << "Exited order maker";
}

}  // namespace order_maker