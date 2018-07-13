// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef INTERFACES_ACCOUNT_ACCOUNT_POOL_H_
#define INTERFACES_ACCOUNT_ACCOUNT_POOL_H_

#include <string>
#include <unordered_map>

#include "interfaces/account/account_operator.h"

namespace interface {
typedef std::unordered_map<std::string, std::unique_ptr<AccountOperator>>
    IdToAccountOperator;

class AccountPool {
 public:
  AccountPool();
  virtual ~AccountPool();

  IdToAccountOperator::iterator begin();
  IdToAccountOperator::iterator end();

  size_t Size();

  AccountOperator* GetAccountOperatorByAccountId(const std::string& account_id);
  AccountOperator* GetAccountOperatorByOrderId(const int64_t order_id);

  void Insert(const std::string& id, AccountOperator* account_operator);

 private:
  IdToAccountOperator id_to_account_operator_;
};

}  // namespace interface

#endif  // INTERFACES_ACCOUNT_ACCOUNT_POOL_H_
