// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "interfaces/order/order_factory.h"

#include <boost/test/unit_test.hpp>

namespace interface {

BOOST_AUTO_TEST_CASE(OrderFactoryTest) {
  OrderFactory::SetOrderId(7);

  Order order = OrderFactory::GetANewOrder();

  BOOST_CHECK_EQUAL(7, order.id());
}

}  // namespace interface
