// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "interfaces/account/account_operator.h"

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include "utils/date_time.h"

using boost::posix_time::microsec_clock;
using google::protobuf::TextFormat;
using interface::CANCELED_ORDER;
using interface::CLOSE;
using interface::CLOSE_TODAY;
using interface::CLOSE_YESTERDAY;
using interface::ERROR_ORDER;
using interface::LONG;
using interface::OPEN;
using interface::SHORT;
using interface::TRADED_ORDER;
using interface::WAITING_FOR_CONFIRM_ORDER;
using std::string;
using std::vector;
using util::TheSameDay;
using util::ToString;

namespace interface {

AccountOperator::AccountOperator(const std::string& account_string) {
  TextFormat::ParseFromString(account_string, &account_);
}

AccountOperator::~AccountOperator() {}

void AccountOperator::Lock() { mutex_.lock(); }

void AccountOperator::Unlock() { mutex_.unlock(); }

const Account& AccountOperator::GetAccount() { return account_; }

string AccountOperator::GetAccountId() { return account_.id(); }

string AccountOperator::GetStrategyName() { return account_.strategy_name(); }

bool AccountOperator::FoundOrder(const int64_t order_id) {
  if (account_.order_id_to_order().count(order_id) == 0 &&
      account_.order_id_to_executed_orders().count(order_id) == 0 &&
      account_.order_id_to_finished_orders().count(order_id) == 0) {
    return false;
  }

  return true;
}

bool AccountOperator::FoundNewOrder(const int64_t order_id) {
  if (account_.order_id_to_order().count(order_id) == 0) {
    return false;
  }

  return true;
}

vector<int64_t> AccountOperator::FindPendingOrders() {
  vector<int64_t> results;

  for (const auto& order_id_to_order : account_.order_id_to_order()) {
    if (order_id_to_order.second.status() == NEW_ORDER ||
        order_id_to_order.second.status() == ORDER_SENT_TO_EXCHANGE) {
      results.push_back(order_id_to_order.first);
    }
  }

  return results;
}

void AccountOperator::InsertOrder(const Order& order) {
  (*account_.mutable_order_id_to_order())[order.id()] = order;
}

void AccountOperator::UpdateCancelOrder(const int64_t order_id) {
  if (account_.order_id_to_order().count(order_id) != 0) {
    account_.mutable_order_id_to_order()->at(order_id).set_status(
        CANCELED_ORDER);
    (*(*account_.mutable_order_id_to_finished_orders())[order_id].add_order()) =
        account_.order_id_to_order().at(order_id);
    account_.mutable_order_id_to_order()->erase(order_id);
  }

  if (account_.order_id_to_executed_orders().count(order_id) != 0) {
    for (const auto& order :
         account_.mutable_order_id_to_executed_orders()->at(order_id).order()) {
      Order* finished_order = account_.mutable_order_id_to_finished_orders()
                                  ->at(order_id)
                                  .add_order();
      (*finished_order) = order;
      finished_order->set_status(CANCELED_ORDER);
    }
    account_.mutable_order_id_to_executed_orders()->erase(order_id);
  }
}

void AccountOperator::UpdateErrorOrder(const int64_t order_id) {
  if (account_.order_id_to_order().count(order_id) != 0) {
    account_.mutable_order_id_to_order()->at(order_id).set_status(ERROR_ORDER);
    (*(*account_.mutable_order_id_to_finished_orders())[order_id].add_order()) =
        account_.order_id_to_order().at(order_id);
    account_.mutable_order_id_to_order()->erase(order_id);
  }

  if (account_.order_id_to_executed_orders().count(order_id) != 0) {
    for (const auto& order :
         account_.mutable_order_id_to_executed_orders()->at(order_id).order()) {
      Order* finished_order = account_.mutable_order_id_to_finished_orders()
                                  ->at(order_id)
                                  .add_order();
      (*finished_order) = order;
      finished_order->set_status(ERROR_ORDER);
    }
    account_.mutable_order_id_to_executed_orders()->erase(order_id);
  }
}

void AccountOperator::UpdateOrderStatus(const int64_t order_id,
                                        const OrderStatus order_status) {
  if (order_status == CANCELED_ORDER || order_status == ERROR_ORDER) {
    if (account_.order_id_to_order().count(order_id) != 0) {
      Order* finished_order =
          (*account_.mutable_order_id_to_finished_orders())[order_id]
              .add_order();
      (*finished_order) = account_.order_id_to_order().at(order_id);
      finished_order->set_status(order_status);
      account_.mutable_order_id_to_order()->erase(order_id);
    }

    if (account_.order_id_to_executed_orders().count(order_id) != 0) {
      for (const auto& order :
           account_.order_id_to_executed_orders().at(order_id).order()) {
        Order* finished_order =
            (*account_.mutable_order_id_to_finished_orders())[order_id]
                .add_order();
        *finished_order = order;
        finished_order->set_status(order_status);
      }

      account_.mutable_order_id_to_executed_orders()->erase(order_id);
    }
    return;
  }

  if (account_.order_id_to_order().count(order_id) != 0) {
    (*account_.mutable_order_id_to_order())[order_id].set_status(order_status);
  }

  if (account_.order_id_to_executed_orders().count(order_id) != 0) {
    for (auto& order :
         (*(*account_.mutable_order_id_to_executed_orders())[order_id]
               .mutable_order())) {
      order.set_status(order_status);
    }
  }
}

void AccountOperator::UpdateTradedOrder(const int64_t order_id,
                                        const int64_t total_quantity,
                                        const int64_t traded_quantity,
                                        const int64_t left_quantity) {
  if (account_.order_id_to_order().count(order_id) == 0) {
    return;
  }

  if (account_.order_id_to_order().at(order_id).quantity() <= left_quantity) {
    return;
  }

  if (left_quantity == 0) {
    Order* order =
        (*account_.mutable_order_id_to_executed_orders())[order_id].add_order();
    *order = account_.order_id_to_order().at(order_id);
    order->set_status(WAITING_FOR_CONFIRM_ORDER);
    account_.mutable_order_id_to_order()->erase(order_id);
  } else {
    int64_t diff =
        account_.order_id_to_order().at(order_id).quantity() - left_quantity;
    Order* order =
        (*account_.mutable_order_id_to_executed_orders())[order_id].add_order();
    *order = account_.order_id_to_order().at(order_id);
    order->set_quantity(diff);
    order->set_status(WAITING_FOR_CONFIRM_ORDER);
    (*account_.mutable_order_id_to_order())[order_id].set_quantity(
        left_quantity);
  }
}

void AccountOperator::ConfirmTradedOrder(const int64_t order_id,
                                         const int64_t traded_quantity,
                                         const double traded_price) {
  if (account_.order_id_to_executed_orders().count(order_id) == 0) {
    return;
  }

  auto orders = account_.order_id_to_executed_orders().at(order_id).order();
  orders.Clear();

  int64_t quantity = traded_quantity;

  for (auto& order :
       account_.order_id_to_executed_orders().at(order_id).order()) {
    if (order.quantity() == quantity &&
        order.status() == WAITING_FOR_CONFIRM_ORDER) {
      quantity = 0;
      UpdatePosition(order.security_id(), traded_quantity, traded_price,
                     order.action(), order.direction());

      Order* finished_order =
          (*account_.mutable_order_id_to_finished_orders())[order_id]
              .add_order();
      *finished_order = order;
      finished_order->set_price(traded_price);
      finished_order->set_status(TRADED_ORDER);
    } else {
      (*orders.Add()) = order;
    }
  }

  if (orders.size() == 0) {
    account_.mutable_order_id_to_executed_orders()->erase(order_id);
  } else {
    (*(*account_.mutable_order_id_to_executed_orders())[order_id]
          .mutable_order()) = orders;
  }
}

// TODO(caixuanting): add multiplier.
void AccountOperator::UpdatePosition(
    const string& security_id, const int64_t traded_quantity,
    const double traded_price, const interface::OrderAction order_action,
    const interface::OrderDirection order_direction) {
  if (order_action == BUY) {
    if (order_direction == OPEN) {
      Position position;
      position.set_security_id(security_id);
      position.set_quantity(traded_quantity);
      position.set_price(traded_price);
      position.set_timestamp(ToString(microsec_clock::local_time()));
      position.set_type(LONG);

      (*(*account_.mutable_security_id_to_position_pool())[security_id]
            .add_position()) = position;
    } else if (order_direction == CLOSE) {
      auto positions =
          account_.security_id_to_position_pool().at(security_id).position();
      positions.Clear();

      int64_t quantity = traded_quantity;

      for (const auto& position :
           account_.security_id_to_position_pool().at(security_id).position()) {
        if (position.type() == SHORT && quantity > 0) {
          if (position.quantity() > quantity) {
            Position* left_position = positions.Add();
            *left_position = position;
            left_position->set_quantity(position.quantity() - quantity);
            account_.set_cash(account_.cash() +
                              quantity * (position.price() - traded_price));
          } else {
            account_.set_cash(account_.cash() +
                              position.quantity() *
                                  (position.price() - traded_price));
          }

          quantity -= position.quantity();
        } else {
          (*positions.Add()) = position;
        }
      }

      if (positions.size() == 0) {
        account_.mutable_security_id_to_position_pool()->erase(security_id);
      } else {
        (*(*account_.mutable_security_id_to_position_pool())[security_id]
              .mutable_position()) = positions;
      }
    } else if (order_direction == CLOSE_TODAY) {
      auto positions =
          account_.security_id_to_position_pool().at(security_id).position();
      positions.Clear();

      int64_t quantity = traded_quantity;

      for (const auto& position :
           account_.security_id_to_position_pool().at(security_id).position()) {
        if (position.type() == SHORT && quantity > 0 &&
            TheSameDay(position.timestamp(),
                       ToString(microsec_clock::local_time()))) {
          if (position.quantity() > quantity) {
            Position* left_position = positions.Add();
            *left_position = position;
            left_position->set_quantity(position.quantity() - quantity);
            account_.set_cash(account_.cash() +
                              quantity * (position.price() - traded_price));
          } else {
            account_.set_cash(account_.cash() +
                              position.quantity() *
                                  (position.price() - traded_price));
          }

          quantity -= position.quantity();
        } else {
          (*positions.Add()) = position;
        }
      }

      if (positions.size() == 0) {
        account_.mutable_security_id_to_position_pool()->erase(security_id);
      } else {
        (*(*account_.mutable_security_id_to_position_pool())[security_id]
              .mutable_position()) = positions;
      }
    } else {
      auto positions =
          account_.security_id_to_position_pool().at(security_id).position();
      positions.Clear();

      int64_t quantity = traded_quantity;

      for (const auto& position :
           account_.security_id_to_position_pool().at(security_id).position()) {
        if (position.type() == SHORT && quantity > 0 &&
            !TheSameDay(position.timestamp(),
                        ToString(microsec_clock::local_time()))) {
          if (position.quantity() > quantity) {
            Position* left_position = positions.Add();
            *left_position = position;
            left_position->set_quantity(position.quantity() - quantity);
            account_.set_cash(account_.cash() +
                              quantity * (position.price() - traded_price));
          } else {
            account_.set_cash(account_.cash() +
                              position.quantity() *
                                  (position.price() - traded_price));
          }

          quantity -= position.quantity();
        } else {
          (*positions.Add()) = position;
        }
      }

      if (positions.size() == 0) {
        account_.mutable_security_id_to_position_pool()->erase(security_id);
      } else {
        (*(*account_.mutable_security_id_to_position_pool())[security_id]
              .mutable_position()) = positions;
      }
    }
  } else {
    if (order_direction == OPEN) {
      Position position;
      position.set_security_id(security_id);
      position.set_quantity(traded_quantity);
      position.set_price(traded_price);
      position.set_timestamp(ToString(microsec_clock::local_time()));
      position.set_type(SHORT);

      (*(*account_.mutable_security_id_to_position_pool())[security_id]
            .add_position()) = position;
    } else if (order_direction == CLOSE) {
      auto positions =
          account_.security_id_to_position_pool().at(security_id).position();
      positions.Clear();

      int64_t quantity = traded_quantity;

      for (const auto& position :
           account_.security_id_to_position_pool().at(security_id).position()) {
        if (position.type() == LONG && quantity > 0) {
          if (position.quantity() > quantity) {
            Position* left_position = positions.Add();
            *left_position = position;
            left_position->set_quantity(position.quantity() - quantity);
            account_.set_cash(account_.cash() -
                              quantity * (position.price() - traded_price));
          } else {
            account_.set_cash(account_.cash() -
                              position.quantity() *
                                  (position.price() - traded_price));
          }

          quantity -= position.quantity();
        } else {
          (*positions.Add()) = position;
        }
      }

      if (positions.size() == 0) {
        account_.mutable_security_id_to_position_pool()->erase(security_id);
      } else {
        (*(*account_.mutable_security_id_to_position_pool())[security_id]
              .mutable_position()) = positions;
      }
    } else if (order_direction == CLOSE_TODAY) {
      auto positions =
          account_.security_id_to_position_pool().at(security_id).position();
      positions.Clear();

      int64_t quantity = traded_quantity;

      for (const auto& position :
           account_.security_id_to_position_pool().at(security_id).position()) {
        if (position.type() == LONG && quantity > 0 &&
            TheSameDay(position.timestamp(),
                       ToString(microsec_clock::local_time()))) {
          if (position.quantity() > quantity) {
            Position* left_position = positions.Add();
            *left_position = position;
            left_position->set_quantity(position.quantity() - quantity);
            account_.set_cash(account_.cash() -
                              quantity * (position.price() - traded_price));
          } else {
            account_.set_cash(account_.cash() -
                              position.quantity() *
                                  (position.price() - traded_price));
          }

          quantity -= position.quantity();
        } else {
          (*positions.Add()) = position;
        }
      }

      if (positions.size() == 0) {
        account_.mutable_security_id_to_position_pool()->erase(security_id);
      } else {
        (*(*account_.mutable_security_id_to_position_pool())[security_id]
              .mutable_position()) = positions;
      }
    } else {
      auto positions =
          account_.security_id_to_position_pool().at(security_id).position();
      positions.Clear();

      int64_t quantity = traded_quantity;

      for (const auto& position :
           account_.security_id_to_position_pool().at(security_id).position()) {
        if (position.type() == LONG && quantity > 0 &&
            !TheSameDay(position.timestamp(),
                        ToString(microsec_clock::local_time()))) {
          if (position.quantity() > quantity) {
            Position* left_position = positions.Add();
            *left_position = position;
            left_position->set_quantity(position.quantity() - quantity);
            account_.set_cash(account_.cash() -
                              quantity * (position.price() - traded_price));
          } else {
            account_.set_cash(account_.cash() -
                              position.quantity() *
                                  (position.price() - traded_price));
          }

          quantity -= position.quantity();
        } else {
          (*positions.Add()) = position;
        }
      }

      if (positions.size() == 0) {
        account_.mutable_security_id_to_position_pool()->erase(security_id);
      } else {
        (*(*account_.mutable_security_id_to_position_pool())[security_id]
              .mutable_position()) = positions;
      }
    }
  }
}

}  // namespace interface
