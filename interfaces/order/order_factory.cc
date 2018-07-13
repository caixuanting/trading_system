// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "interfaces/order/order_factory.h"

namespace interface {

int64_t OrderFactory::order_id_ = 0;

void OrderFactory::SetOrderId(const int64_t order_id) { order_id_ = order_id; }

Order OrderFactory::GetANewOrder() {
  Order order;

  order.set_id(order_id_++);

  return order;
}

}  // namespace interface