// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef INTERFACES_ACCOUNT_ACCOUNT_H_
#define INTERFACES_ACCOUNT_ACCOUNT_H_

#include <mutex>
#include <string>
#include <unordered_map>

#include "interfaces/account/proto/account.pb.h"

namespace interface {

// Lock before using any method in this class.
class AccountOperator {
 public:
  AccountOperator(const std::string& account_string);
  virtual ~AccountOperator();

  void Lock();
  void Unlock();

  const Account& GetAccount();
  std::string GetAccountId();
  std::string GetStrategyName();

  bool FoundOrder(const int64_t order_id);
  bool FoundNewOrder(const int64_t order_id);
  std::vector<int64_t> FindPendingOrders();

  void InsertOrder(const Order& order);
  void UpdateCancelOrder(const int64_t order_id);
  void UpdateErrorOrder(const int64_t order_id);
  void UpdateOrderStatus(const int64_t order_id,
                         const OrderStatus order_status);
  void UpdateTradedOrder(const int64_t order_id, const int64_t total_quantity,
                         const int64_t traded_quantity,
                         const int64_t left_quantity);
  void ConfirmTradedOrder(const int64_t order_id, const int64_t traded_quantity,
                          const double traded_price);
  void UpdatePosition(const std::string& security_id,
                      const int64_t traded_quantity, const double traded_price,
                      const interface::OrderAction order_action,
                      const interface::OrderDirection order_direction);

 private:
  Account account_;
  std::mutex mutex_;
};

}  // namespace interface

#endif  // INTERFACES_ACCOUNT_ACCOUNT_H_
