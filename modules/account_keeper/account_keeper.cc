// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "modules/account_keeper/account_keeper.h"

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include "utils/database_operator.h"

using google::protobuf::TextFormat;
using interface::Account;
using interface::AccountOperator;
using interface::AccountPool;
using sql::ResultSet;
using std::string;
using std::thread;
using std::unique_ptr;
using util::DatabaseOperator;
using util::Queue;

namespace account_keeper {
namespace {

void WriteAccountToDatabase(AccountOperator* account_operator,
                            DatabaseOperator* database_operator) {
  if (account_operator == nullptr) {
    LOG(ERROR) << "Account operator is nullptr";

    return;
  }

  Account account = account_operator->GetAccount();
  account.clear_order_id_to_finished_orders();

  TextFormat::Printer printer;
  printer.SetSingleLineMode(true);

  string account_string;
  printer.PrintToString(account, &account_string);

  string command = "INSERT INTO accounts VALUES('";
  command += account.id();
  command += "','";
  command += account_string;
  command += "') ON DUPLICATE KEY UPDATE account='";
  command += account_string;
  command += "'";
  database_operator->Write(command);

  LOG(INFO) << "Write account <" << account.id() << "> to database";
}

}  // namespace

AccountKeeper::AccountKeeper(const AccountKeeperConfig& config,
                             AccountPool* account_pool,
                             Queue<AccountMessage>* account_message_queue)
    : config_(config),
      account_pool_(account_pool),
      account_message_queue_(account_message_queue) {}

AccountKeeper::~AccountKeeper() {
  unique_ptr<DatabaseOperator> database_operator(
      new DatabaseOperator(config_.database_config()));

  if (!database_operator->Init()) {
    LOG(ERROR) << "Failed to init database";
    return;
  }

  for (const auto& id_to_account_operator : *account_pool_) {
    WriteAccountToDatabase(id_to_account_operator.second.get(),
                           database_operator.get());
  }
}

void AccountKeeper::Init() {
  LOG(INFO) << "Initing account keeper";

  DatabaseOperator database_operator(config_.database_config());
  if (!database_operator.Init()) {
    LOG(FATAL) << "Failed to init database";
    exit(1);
  }

  unique_ptr<ResultSet> result_set(
      database_operator.Read("SELECT * FROM accounts"));

  while (result_set->next()) {
    string account_id = result_set->getString("account_id");
    string account = result_set->getString("account");

    AccountOperator* account_operator = new AccountOperator(account);
    account_pool_->Insert(account_id, account_operator);

    LOG(INFO) << "Created account with id <" << account_id << "> value <"
              << account << ">";
  }

  LOG(INFO) << "Inited account keeper";
}

void AccountKeeper::Start() {
  LOG(INFO) << "Starting account keeper";

  message_thread_ = thread([this] {
    while (this->running_) {
      this->account_message_queue_->Wait();

      unique_ptr<DatabaseOperator> database_operator(
          new DatabaseOperator(config_.database_config()));

      if (!database_operator->Init()) {
        LOG(ERROR) << "Failed to init database";

        continue;
      }

      while (!this->account_message_queue_->Empty()) {
        AccountMessage account_message = this->account_message_queue_->Next();
        const string& account_id = account_message.account_id();
        AccountOperator* account_operator =
            this->account_pool_->GetAccountOperatorByAccountId(account_id);

        if (account_operator == nullptr) {
          LOG(ERROR) << "account operator not found for account id <"
                     << account_id << ">";

          continue;
        }

        WriteAccountToDatabase(account_operator, database_operator.get());
      }
    }
  });

  LOG(INFO) << "Started account keeper";
}

void AccountKeeper::Exit() {
  LOG(INFO) << "Exiting account keeper";

  running_ = false;
  account_message_queue_->Notify();
  message_thread_.join();

  account_message_queue_->Clear();

  LOG(INFO) << "Exited account keeper";
}

}  // namespace account_keeper
