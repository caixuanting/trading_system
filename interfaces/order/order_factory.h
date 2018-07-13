// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef INTERFACES_ORDER_ORDER_FACTORY_H_
#define INTERFACES_ORDER_ORDER_FACTORY_H_

#include "interfaces/order/proto/order.pb.h"

namespace interface {

class OrderFactory {
 public:
  static void SetOrderId(const int64_t order_id);

  static Order GetANewOrder();

 private:
  static int64_t order_id_;
};

}  // namespace interface

#endif  // INTERFACES_ORDER_ORDER_FACTORY_H_