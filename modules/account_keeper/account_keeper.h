// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef MODULES_ACCOUNT_KEEPER_ACCOUNT_KEEPER_H_
#define MODULES_ACCOUNT_KEEPER_ACCOUNT_KEEPER_H_

#include <thread>

#include "interfaces/account/account_pool.h"
#include "modules/account_keeper/proto/account_keeper_config.pb.h"
#include "modules/account_keeper/proto/account_message.pb.h"
#include "utils/queue.h"

namespace account_keeper {

class AccountKeeper {
 public:
  AccountKeeper(const AccountKeeperConfig& config,
                interface::AccountPool* account_pool,
                util::Queue<AccountMessage>* account_message_queue);
  virtual ~AccountKeeper();

  // Read all accounts from database and construct in memory instances.
  void Init();
  void Start();
  void Exit();

 private:
  AccountKeeperConfig config_;
  bool running_ = true;
  interface::AccountPool* account_pool_;  // Not owned.
  std::thread message_thread_;
  util::Queue<AccountMessage>* account_message_queue_;  // Not owned.
};

}  // namespace account_keeper

#endif  // MODULES_ACCOUNT_KEEPER_ACCOUNT_KEEPER_H_