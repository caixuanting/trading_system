// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef MODULES_ORDER_MAKER_ORDER_MAKER_H_
#define MODULES_ORDER_MAKER_ORDER_MAKER_H_

#include <unordered_map>

#include "modules/order_maker/clients/ctp_future_order_client.h"
#include "modules/order_maker/proto/order_maker_config.pb.h"

namespace order_maker {

typedef std::unordered_map<std::string, std::unique_ptr<OrderClient>>
    NameToOrderClient;
typedef std::unordered_map<std::string,
                           std::unique_ptr<util::Queue<OrderMessage>>>
    NameToOrderMessageQueue;

class OrderMaker {
 public:
  OrderMaker(
      const OrderMakerConfig& config, interface::AccountPool* account_pool,
      util::Queue<OrderMessage>* order_message_queue,
      util::Queue<strategy_runner::StrategyMessage>* strategy_message_queue,
      util::Queue<event_scheduler::Event*>* event_queue);
  virtual ~OrderMaker();

  void Init();
  void Start();
  void Exit();

 private:
  NameToOrderClient name_to_order_client_;
  NameToOrderMessageQueue name_to_order_message_queue_;
  OrderMakerConfig config_;
  bool running_ = true;
  interface::AccountPool* account_pool_;  // Not owned.
  std::thread order_message_thread_;
  std::vector<std::thread> client_threads_;
  util::Queue<OrderMessage>* order_message_queue_;     // Not owned.
  util::Queue<event_scheduler::Event*>* event_queue_;  // Not owned.
  util::Queue<strategy_runner::StrategyMessage>*
      strategy_message_queue_;  // Not owned.
};

}  // order_maker

#endif  // MODULES_ORDER_MAKER_ORDER_MAKER_H_