// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "interfaces/account/account_pool.h"

#include <glog/logging.h>

using std::string;

namespace interface {

AccountPool::AccountPool() {}

AccountPool::~AccountPool() {}

IdToAccountOperator::iterator AccountPool::begin() {
  return id_to_account_operator_.begin();
}

IdToAccountOperator::iterator AccountPool::end() {
  return id_to_account_operator_.end();
}

size_t AccountPool::Size() { return id_to_account_operator_.size(); }

AccountOperator* AccountPool::GetAccountOperatorByAccountId(
    const string& account_id) {
  if (id_to_account_operator_.count(account_id) == 0) {
    LOG(ERROR) << "Account " << account_id << " not found";

    return nullptr;
  } else {
    return id_to_account_operator_[account_id].get();
  }
}

AccountOperator* AccountPool::GetAccountOperatorByOrderId(
    const int64_t order_id) {
  for (const auto& iter : id_to_account_operator_) {
    if (iter.second->FoundOrder(order_id)) {
      return iter.second.get();
    }
  }

  LOG(WARNING) << "Account not found for order " << order_id;
  return nullptr;
}

void AccountPool::Insert(const string& id, AccountOperator* account_operator) {
  id_to_account_operator_[id].reset(account_operator);
}

}  // namespace interface
