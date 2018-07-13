// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "interfaces/account/account_operator.h"

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <google/protobuf/text_format.h>

using google::protobuf::TextFormat;
using interface::BUY;
using interface::CANCELED_ORDER;
using interface::ERROR_ORDER;
using interface::NEW_ORDER;
using interface::OPEN;
using std::string;
using std::vector;

namespace interface {

struct AccountOperatorFixture {
  AccountOperatorFixture() {
    string account_string =
        R"(id: "test_account")"
        R"(cash: 0)"
        R"(security_id_to_position_pool {)"
        R"(  key: "test_security_id")"
        R"(  value {)"
        R"(    position {)"
        R"(      security_id: "test_security_id")"
        R"(      quantity: 100)"
        R"(      price: 9.77)"
        R"(      timestamp: "2016-01-01 10:00:00")"
        R"(      type: SHORT)"
        R"(    })"
        R"(    position {)"
        R"(      security_id: "test_security_id")"
        R"(      quantity: 100)"
        R"(      price: 9.77)"
        R"(      timestamp: "2016-01-02 10:01:00")"
        R"(      type: LONG)"
        R"(    })"
        R"(  })"
        R"(})"
        R"(order_id_to_order {)"
        R"(  key: 1)"
        R"(  value {)"
        R"(    id: 1)"
        R"(    security_id: "test_security_id")"
        R"(    status: NEW_ORDER)"
        R"(    action: BUY)"
        R"(    direction: OPEN)"
        R"(    quantity: 100)"
        R"(    price: 9.78)"
        R"(  })"
        R"(})"
        R"(order_id_to_executed_orders {)"
        R"(  key: 2)"
        R"(  value {)"
        R"(    order {)"
        R"(      id: 2)"
        R"(      security_id: "test_security_id")"
        R"(      action: BUY)"
        R"(      direction: OPEN)"
        R"(      quantity: 50)"
        R"(      price: 9.78)"
        R"(      status: WAITING_FOR_CONFIRM_ORDER)"
        R"(    })"
        R"(  })"
        R"(})"
        R"(order_id_to_executed_orders {)"
        R"(  key: 3)"
        R"(  value {)"
        R"(    order {)"
        R"(      id: 3)"
        R"(      security_id: "test_security_id")"
        R"(      action: BUY)"
        R"(      direction: CLOSE)"
        R"(      quantity: 50)"
        R"(      price: 9.78)"
        R"(      status: WAITING_FOR_CONFIRM_ORDER)"
        R"(    })"
        R"(  })"
        R"(})"
        R"(order_id_to_executed_orders {)"
        R"(  key: 4)"
        R"(  value {)"
        R"(    order {)"
        R"(      id: 4)"
        R"(      security_id: "test_security_id")"
        R"(      action: BUY)"
        R"(      direction: CLOSE_TODAY)"
        R"(      quantity: 50)"
        R"(      price: 9.78)"
        R"(      status: WAITING_FOR_CONFIRM_ORDER)"
        R"(    })"
        R"(  })"
        R"(})"
        R"(order_id_to_executed_orders {)"
        R"(  key: 5)"
        R"(  value {)"
        R"(    order {)"
        R"(      id: 5)"
        R"(      security_id: "test_security_id")"
        R"(      action: BUY)"
        R"(      direction: CLOSE_YESTERDAY)"
        R"(      quantity: 50)"
        R"(      price: 9.78)"
        R"(      status: WAITING_FOR_CONFIRM_ORDER)"
        R"(    })"
        R"(  })"
        R"(})"
        R"(order_id_to_executed_orders {)"
        R"(  key: 6)"
        R"(  value {)"
        R"(    order {)"
        R"(      id: 6)"
        R"(      security_id: "test_security_id")"
        R"(      action: SELL)"
        R"(      direction: OPEN)"
        R"(      quantity: 50)"
        R"(      price: 9.78)"
        R"(      status: WAITING_FOR_CONFIRM_ORDER)"
        R"(    })"
        R"(  })"
        R"(})"
        R"(order_id_to_executed_orders {)"
        R"(  key: 7)"
        R"(  value {)"
        R"(    order {)"
        R"(      id: 7)"
        R"(      security_id: "test_security_id")"
        R"(      action: SELL)"
        R"(      direction: CLOSE)"
        R"(      quantity: 50)"
        R"(      price: 9.78)"
        R"(      status: WAITING_FOR_CONFIRM_ORDER)"
        R"(    })"
        R"(  })"
        R"(})"
        R"(order_id_to_executed_orders {)"
        R"(  key: 8)"
        R"(  value {)"
        R"(    order {)"
        R"(      id: 8)"
        R"(      security_id: "test_security_id")"
        R"(      action: SELL)"
        R"(      direction: CLOSE_TODAY)"
        R"(      status: WAITING_FOR_CONFIRM_ORDER)"
        R"(      quantity: 50)"
        R"(      price: 9.78)"
        R"(    })"
        R"(  })"
        R"(})"
        R"(order_id_to_executed_orders {)"
        R"(  key: 9)"
        R"(  value {)"
        R"(    order {)"
        R"(      id: 9)"
        R"(      security_id: "test_security_id")"
        R"(      action: SELL)"
        R"(      direction: CLOSE_YESTERDAY)"
        R"(      status: WAITING_FOR_CONFIRM_ORDER)"
        R"(      quantity: 50)"
        R"(      price: 9.78)"
        R"(    })"
        R"(  })"
        R"(})"
        R"(order_id_to_finished_orders {)"
        R"(  key: 10)"
        R"(  value {)"
        R"(    order {)"
        R"(      id: 10)"
        R"(      security_id: "test_security_id")"
        R"(      quantity: 50)"
        R"(      price: 9.74)"
        R"(    })"
        R"(    order {)"
        R"(      id: 10)"
        R"(      security_id: "test_security_id")"
        R"(      quantity: 50)"
        R"(      price: 9.74)"
        R"(    })"
        R"(  })"
        R"(})";

    account_operator.reset(new AccountOperator(account_string));
  }

  virtual ~AccountOperatorFixture() {}

  std::unique_ptr<AccountOperator> account_operator;
};

BOOST_FIXTURE_TEST_SUITE(AccountOperatorTests, AccountOperatorFixture)

BOOST_AUTO_TEST_CASE(FoundOrderTest) {
  BOOST_CHECK_EQUAL(true, account_operator->FoundOrder(1));
  BOOST_CHECK_EQUAL(true, account_operator->FoundOrder(2));
  BOOST_CHECK_EQUAL(true, account_operator->FoundOrder(3));
  BOOST_CHECK_EQUAL(true, account_operator->FoundOrder(4));
  BOOST_CHECK_EQUAL(true, account_operator->FoundOrder(5));
  BOOST_CHECK_EQUAL(true, account_operator->FoundOrder(6));
  BOOST_CHECK_EQUAL(true, account_operator->FoundOrder(7));
  BOOST_CHECK_EQUAL(true, account_operator->FoundOrder(8));
  BOOST_CHECK_EQUAL(true, account_operator->FoundOrder(9));
  BOOST_CHECK_EQUAL(true, account_operator->FoundOrder(10));
  BOOST_CHECK_EQUAL(false, account_operator->FoundOrder(11));
}

BOOST_AUTO_TEST_CASE(FindPendingOrderTest) {
  vector<int64_t> results = account_operator->FindPendingOrder();

  BOOST_CHECK_EQUAL(1, results.size());
  BOOST_CHECK_EQUAL(1, results.back());
}

BOOST_AUTO_TEST_CASE(InsertOrderTest) {
  account_operator->Lock();

  Order order;

  TextFormat::ParseFromString(
      R"(id: 5)"
      R"(quantity: 100)"
      R"(price: 9.74)",
      &order);

  account_operator->InsertOrder(order);

  const Account& account = account_operator->GetAccount();

  Order result = account.order_id_to_order().at(5);

  BOOST_CHECK_EQUAL(5, result.id());
  BOOST_CHECK_EQUAL(NEW_ORDER, result.status());

  account_operator->Unlock();
}

BOOST_AUTO_TEST_CASE(UpdateCancelOrderTest) {
  account_operator->Lock();

  account_operator->UpdateCancelOrder(1);

  const Account& account = account_operator->GetAccount();

  Order result = account.order_id_to_finished_orders().at(1).order().Get(0);

  BOOST_CHECK_EQUAL(1, result.id());
  BOOST_CHECK_EQUAL(CANCELED_ORDER, result.status());

  account_operator->Unlock();
}

BOOST_AUTO_TEST_CASE(UpdateErrorOrderTest) {
  account_operator->Lock();

  account_operator->UpdateErrorOrder(1);

  const Account& account = account_operator->GetAccount();

  Order result = account.order_id_to_finished_orders().at(1).order().Get(0);

  BOOST_CHECK_EQUAL(1, result.id());
  BOOST_CHECK_EQUAL(ERROR_ORDER, result.status());

  account_operator->Unlock();
}

BOOST_AUTO_TEST_CASE(UpdateOrderStatusTest) {
  account_operator->Lock();

  account_operator->UpdateOrderStatus(1, CANCELED_ORDER);
  account_operator->UpdateOrderStatus(2, ERROR_ORDER);

  const Account& account = account_operator->GetAccount();

  {
    Order result = account.order_id_to_finished_orders().at(1).order().Get(0);

    BOOST_CHECK_EQUAL(1, result.id());
    BOOST_CHECK_EQUAL(CANCELED_ORDER, result.status());
  }

  {
    Order result = account.order_id_to_finished_orders().at(2).order().Get(0);

    BOOST_CHECK_EQUAL(2, result.id());
    BOOST_CHECK_EQUAL(ERROR_ORDER, result.status());
  }

  account_operator->Unlock();
}

BOOST_AUTO_TEST_CASE(UpdateTradedOrderTest) {
  account_operator->Lock();

  account_operator->UpdateTradedOrder(1, 100, 10, 90);

  {
    const Account& account = account_operator->GetAccount();
    Order result = account.order_id_to_executed_orders().at(1).order().Get(0);

    BOOST_CHECK_EQUAL(1, result.id());
    BOOST_CHECK_EQUAL(10, result.quantity());
    BOOST_CHECK_EQUAL(WAITING_FOR_CONFIRM_ORDER, result.status());
  }

  account_operator->UpdateTradedOrder(1, 100, 20, 80);

  {
    const Account& account = account_operator->GetAccount();
    Order result = account.order_id_to_executed_orders().at(1).order().Get(1);

    BOOST_CHECK_EQUAL(1, result.id());
    BOOST_CHECK_EQUAL(10, result.quantity());
    BOOST_CHECK_EQUAL(WAITING_FOR_CONFIRM_ORDER, result.status());
  }

  account_operator->Unlock();
}

BOOST_AUTO_TEST_CASE(OpenLogOrderTest) {
  account_operator->Lock();

  account_operator->ConfirmTradedOrder(2, 50, 7.0);

  {
    const Account& account = account_operator->GetAccount();
    Order result_order =
        account.order_id_to_finished_orders().at(2).order().Get(0);

    BOOST_CHECK_EQUAL(2, result_order.id());
    BOOST_CHECK_EQUAL(50, result_order.quantity());
    BOOST_CHECK(fabs(7.0 - result_order.price()) < 0.001);
    BOOST_CHECK_EQUAL(TRADED_ORDER, result_order.status());

    Position result_position = account.security_id_to_position_pool()
                                   .at("test_security_id")
                                   .position()
                                   .Get(2);

    BOOST_CHECK_EQUAL(50, result_position.quantity());
    BOOST_CHECK_EQUAL(7.0, result_position.price());
    BOOST_CHECK_EQUAL(LONG, result_position.type());
  }

  account_operator->Unlock();
}

BOOST_AUTO_TEST_CASE(CloseLongOrderTest) {
  account_operator->Lock();

  account_operator->ConfirmTradedOrder(3, 50, 7.0);

  {
    const Account& account = account_operator->GetAccount();
    Order result = account.order_id_to_finished_orders().at(3).order().Get(0);

    BOOST_CHECK_EQUAL(3, result.id());
    BOOST_CHECK_EQUAL(50, result.quantity());
    BOOST_CHECK(fabs(7.0 - result.price()) < 0.001);
    BOOST_CHECK_EQUAL(TRADED_ORDER, result.status());

    Position result_position = account.security_id_to_position_pool()
                                   .at("test_security_id")
                                   .position()
                                   .Get(0);

    BOOST_CHECK_EQUAL(50, result_position.quantity());
    BOOST_CHECK_EQUAL(9.77, result_position.price());
    BOOST_CHECK_EQUAL(SHORT, result_position.type());

    BOOST_CHECK(fabs(138.5 - account.cash()) < 0.00001);
  }

  account_operator->Unlock();
}

BOOST_AUTO_TEST_CASE(CloseYesterdayLongOrderTest) {
  account_operator->Lock();

  account_operator->ConfirmTradedOrder(5, 50, 7.0);

  {
    const Account& account = account_operator->GetAccount();
    Order result = account.order_id_to_finished_orders().at(5).order().Get(0);

    BOOST_CHECK_EQUAL(5, result.id());
    BOOST_CHECK_EQUAL(50, result.quantity());
    BOOST_CHECK(fabs(7.0 - result.price()) < 0.001);
    BOOST_CHECK_EQUAL(TRADED_ORDER, result.status());

    Position result_position = account.security_id_to_position_pool()
                                   .at("test_security_id")
                                   .position()
                                   .Get(0);

    BOOST_CHECK_EQUAL(50, result_position.quantity());
    BOOST_CHECK_EQUAL(9.77, result_position.price());
    BOOST_CHECK_EQUAL(SHORT, result_position.type());

    BOOST_CHECK(fabs(138.5 - account.cash()) < 0.00001);
  }

  account_operator->Unlock();
}

BOOST_AUTO_TEST_CASE(CloseShortOrderTest) {
  account_operator->Lock();

  account_operator->ConfirmTradedOrder(7, 50, 7.0);

  {
    const Account& account = account_operator->GetAccount();
    Order result = account.order_id_to_finished_orders().at(7).order().Get(0);

    BOOST_CHECK_EQUAL(7, result.id());
    BOOST_CHECK_EQUAL(50, result.quantity());
    BOOST_CHECK(fabs(7.0 - result.price()) < 0.001);
    BOOST_CHECK_EQUAL(TRADED_ORDER, result.status());

    Position result_position = account.security_id_to_position_pool()
                                   .at("test_security_id")
                                   .position()
                                   .Get(1);

    BOOST_CHECK_EQUAL(50, result_position.quantity());
    BOOST_CHECK_EQUAL(9.77, result_position.price());
    BOOST_CHECK_EQUAL(LONG, result_position.type());

    BOOST_CHECK(fabs(138.5 + account.cash()) < 0.00001);
  }

  account_operator->Unlock();
}

BOOST_AUTO_TEST_CASE(CloseYesterdayShortOrderTest) {
  account_operator->Lock();

  account_operator->ConfirmTradedOrder(9, 50, 7.0);

  {
    const Account& account = account_operator->GetAccount();
    Order result = account.order_id_to_finished_orders().at(9).order().Get(0);

    BOOST_CHECK_EQUAL(9, result.id());
    BOOST_CHECK_EQUAL(50, result.quantity());
    BOOST_CHECK(fabs(7.0 - result.price()) < 0.001);
    BOOST_CHECK_EQUAL(TRADED_ORDER, result.status());

    Position result_position = account.security_id_to_position_pool()
                                   .at("test_security_id")
                                   .position()
                                   .Get(1);

    BOOST_CHECK_EQUAL(50, result_position.quantity());
    BOOST_CHECK_EQUAL(9.77, result_position.price());
    BOOST_CHECK_EQUAL(LONG, result_position.type());

    BOOST_CHECK(fabs(138.5 + account.cash()) < 0.00001);
  }

  account_operator->Unlock();
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace interface
