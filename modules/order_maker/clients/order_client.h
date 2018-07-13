// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef MODULES_ORDER_MAKER_CLIENTS_ORDER_CLIENTS_H_
#define MODULES_ORDER_MAKER_CLIENTS_ORDER_CLIENTS_H_

namespace order_maker {

class OrderClient {
 public:
  OrderClient();
  virtual ~OrderClient();

  virtual void Init() = 0;
  virtual void Start() = 0;
  virtual void Exit() = 0;
};

}  // namespace order_maker

#endif  // MODULES_ORDER_MAKER_CLIENTS_ORDER_CLIENTS_H_