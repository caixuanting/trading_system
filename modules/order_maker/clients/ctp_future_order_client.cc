// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "modules/order_maker/clients/ctp_future_order_client.h"

#include <exception>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include "interfaces/data/proto/security.pb.h"
#include "utils/database_operator.h"

using boost::filesystem::path;
using boost::filesystem::system_complete;
using boost::gregorian::date;
using boost::gregorian::day_clock;
using boost::posix_time::duration_from_string;
using boost::posix_time::ptime;
using boost::posix_time::second_clock;
using boost::posix_time::seconds;
using boost::posix_time::time_duration;
using event_scheduler::Action;
using event_scheduler::Event;
using google::protobuf::TextFormat;
using interface::Account;
using interface::AccountOperator;
using interface::AccountPool;
using interface::BUY;
using interface::CANCEL_SENT_TO_EXCHANGE;
using interface::CLOSE;
using interface::CLOSE_TODAY;
using interface::CLOSE_YESTERDAY;
using interface::CTP_FUTURE;
using interface::LIMIT_PRICE;
using interface::MARKET_PRICE;
using interface::OPEN;
using interface::ORDER_SENT_TO_EXCHANGE;
using interface::Order;
using interface::SELL;
using interface::Security;
using std::endl;
using std::exception;
using std::string;
using std::thread;
using std::to_string;
using std::unique_ptr;
using strategy_runner::ORDER;
using strategy_runner::StrategyMessage;
using util::DatabaseOperator;
using util::Queue;

namespace order_maker {
namespace {

const int32_t kSecondsPerDay = 24 * 3600;
const int32_t kTimeToWaitInSeconds = 2 * 60;
const char kCountry[] = "cn";

}  // namespace

CtpFutureOrderClient::CtpFutureOrderClient(
    const CtpFutureOrderClientConfig& config, AccountPool* account_pool,
    Queue<Event*>* event_queue, Queue<OrderMessage>* order_message_queue,
    Queue<StrategyMessage>* strategy_message_queue)
    : config_(config),
      account_pool_(account_pool),
      order_message_queue_(order_message_queue),
      event_queue_(event_queue),
      strategy_message_queue_(strategy_message_queue) {}

CtpFutureOrderClient::~CtpFutureOrderClient() {}

void CtpFutureOrderClient::Init() {
  // Init trader api.
  try {
    path complete_path(system_complete(config_.log_dir()));
    string complete_path_string = complete_path.string();
    trader_api_ =
        CThostFtdcTraderApi::CreateFtdcTraderApi(complete_path_string.c_str());

    LOG(INFO) << "Ctp future order client <" << config_.name()
              << "> is using Version <" << trader_api_->GetApiVersion()
              << "> api";
  } catch (const exception& e) {
    LOG(FATAL) << "Ctp future order client <" << config_.name()
               << "> failed to create trader api: " << e.what();
    exit(1);
  }

  // Schedule user login, which triggers settlement confirmation.
  date today = day_clock::local_day();

  for (const auto& time_interval : config_.time_interval()) {
    // 2 minute before the market opens.
    time_duration begin_time =
        duration_from_string(time_interval.begin_time()) -
        seconds(kTimeToWaitInSeconds);
    ptime begin_start_time(today, begin_time);

    Action begin_action = [this, begin_time] {
      this->UserLogin();

      LOG(INFO) << "User login for <" << config_.name() << "> at <"
                << to_simple_string(begin_time) << ">";
    };

    unique_ptr<Event> begin_event(
        new Event(begin_action, begin_start_time, kSecondsPerDay,
                  "User login for " + config_.name() + " at " +
                      to_simple_string(begin_time)));
    events_.emplace_back(move(begin_event));

    event_queue_->Insert(events_.back().get());
    event_queue_->Notify();
  }
}

void CtpFutureOrderClient::Start() {
  // Thread runs trader api.
  api_thread_ = thread([this] {
    try {
      this->trader_api_->RegisterSpi(this);
      this->trader_api_->SubscribePublicTopic(THOST_TERT_RESUME);
      this->trader_api_->SubscribePrivateTopic(THOST_TERT_RESUME);

      for (const auto& frontend_address : this->config_.frontend_address()) {
        LOG(INFO) << "Ctp future order client <" << this->config_.name()
                  << "> register to <" << frontend_address << ">";

        this->trader_api_->RegisterFront(
            const_cast<char*>(frontend_address.c_str()));
      }

      this->trader_api_->Init();
      this->trader_api_->Join();
    } catch (const exception& e) {
      LOG(FATAL) << "Start ctp future order client <" << this->config_.name()
                 << "> failed: " << e.what();
      exit(1);
    }
  });

  // Thread runs message processing.
  message_thread_ = thread([this] {
    while (this->running_) {
      order_message_queue_->Wait();

      if (this->running_) {
        while (!order_message_queue_->Empty()) {
          OrderMessage order_message = order_message_queue_->Next();
          LOG(INFO) << order_message.DebugString();

          switch (order_message.command()) {
            case PLACE_ORDER:
              PlaceOrder(order_message);
              break;
            case CANCEL_ORDER:
              CancelOrder(order_message);
              break;
            case REQUEST_ACCOUNT_INFO:
              RequestAccountInfo();
              break;
            case REQUEST_ACCOUNT_DETAIL:
              RequestAccountDetail();
              break;
            case REQUEST_SECURITY_DETAIL:
              RequestSecurityDetail(order_message);
              break;
            default:
              LOG(INFO) << "Order action not recognized";
              break;
          }
        }
      }
    }
  });
}

void CtpFutureOrderClient::Exit() {
  // Stop message thread.
  running_ = false;
  order_message_queue_->Notify();
  message_thread_.join();

  // Stop api thread.
  try {
    trader_api_->RegisterSpi(nullptr);
    trader_api_->Release();
    trader_api_ = nullptr;
  } catch (const exception& e) {
    LOG(FATAL) << "Exit ctp future order client <" << config_.name()
               << "> failed: " << e.what();
    exit(1);
  }
  api_thread_.join();
}

void CtpFutureOrderClient::UserLogin() {
  CThostFtdcReqUserLoginField request;

  memset(&request, 0, sizeof(request));
  strcpy(request.BrokerID, config_.broker_id().c_str());
  strcpy(request.UserID, config_.user_id().c_str());
  strcpy(request.Password, config_.password().c_str());

  int32_t request_id = request_id_++;

  if (trader_api_->ReqUserLogin(&request, request_id) != 0) {
    LOG(ERROR) << "User login request <" << request_id
               << "> for CTP data client failed";
  } else {
    LOG(INFO) << "Request <" << request_id << "> for user login is sent";
  }
}

void CtpFutureOrderClient::SettlementConfirm() {
  CThostFtdcSettlementInfoConfirmField request;

  memset(&request, 0, sizeof(request));
  strcpy(request.BrokerID, config_.broker_id().c_str());
  strcpy(request.InvestorID, config_.user_id().c_str());

  try {
    int32_t request_id = request_id_++;

    LOG(INFO) << "Confirm settlement with request id <" << request_id << ">";

    if (trader_api_->ReqSettlementInfoConfirm(&request, request_id) != 0) {
      LOG(ERROR) << "Confirm settlement for <" << config_.name()
                 << "> is not sent";
    } else {
      LOG(INFO) << "Confirm settlement for <" << config_.name() << "> is sent";
    }
  } catch (const exception& e) {
    LOG(FATAL) << "Confirm settlement for <" << config_.name()
               << "> failed: " << e.what();
    exit(1);
  }
}

void CtpFutureOrderClient::OnFrontConnected() {
  LOG_EVERY_N(INFO, 100) << "Ctp future data client <" << config_.name() << "> connected";

  // We schedule UserLogin before market open.
  // But when the connection is broken during trading hours,
  // we need login again.
  if (IsInTradingHours()) {
    UserLogin();
  }
}

void CtpFutureOrderClient::OnFrontDisconnected(int32_t reason) {
  LOG_EVERY_N(INFO, 100) << "Ctp future data client <" << config_.name() << "> disconnected";
}

void CtpFutureOrderClient::OnRspError(CThostFtdcRspInfoField* response_info,
                                      int32_t request_id, bool is_last) {
  if (response_info != nullptr && response_info->ErrorID != 0) {
    LOG(ERROR) << "Error for request <" << request_id << "> with error id <"
               << response_info->ErrorID << "> error message <"
               << response_info->ErrorMsg << ">";
  } else {
    LOG(INFO) << "No error for request <" << request_id << ">";
  }
}

void CtpFutureOrderClient::OnRspUserLogin(
    CThostFtdcRspUserLoginField* response_user_login,
    CThostFtdcRspInfoField* response_info, int32_t request_id, bool is_last) {
  if ((response_info != nullptr && response_info->ErrorID != 0)) {
    LOG(ERROR) << "User login failed with error id <" << response_info->ErrorID
               << "> error message <" << response_info->ErrorMsg << ">";

    return;
  }

  if (response_user_login == nullptr) {
    LOG(ERROR) << "response_user_login is nullptr";

    return;
  }

  front_id_ = response_user_login->FrontID;
  session_id_ = response_user_login->SessionID;

  LOG(INFO) << "Ctp future order client " << config_.name()
            << " user login successful " << endl
            << "trading day: " << response_user_login->TradingDay << endl
            << "login time: " << response_user_login->LoginTime << endl
            << "broker id: " << response_user_login->BrokerID << endl
            << "user id: " << response_user_login->UserID << endl
            << "system name: " << response_user_login->SystemName << endl
            << "front id: " << response_user_login->FrontID << endl
            << "session id: " << response_user_login->SessionID << endl
            << "max order ref: " << response_user_login->MaxOrderRef << endl
            << "SHFE time: " << response_user_login->SHFETime << endl
            << "DCE time: " << response_user_login->DCETime << endl
            << "CZCE time: " << response_user_login->CZCETime << endl
            << "FFXE time: " << response_user_login->FFEXTime << endl
            << "INE time: " << response_user_login->INETime;

  SettlementConfirm();
}

void CtpFutureOrderClient::OnRspSettlementInfoConfirm(
    CThostFtdcSettlementInfoConfirmField* settlement_info_confirm,
    CThostFtdcRspInfoField* response_info, int32_t request_id, bool is_last) {
  if (response_info != nullptr && response_info->ErrorID != 0) {
    LOG(ERROR) << "Confirm settlement for <" << config_.name()
               << "> failed with error id <" << response_info->ErrorID
               << "> error message <" << response_info->ErrorMsg << ">";
  } else {
    LOG(INFO) << "Confirmed settlement for <" << config_.name() << ">";
  }
}

void CtpFutureOrderClient::OnRspOrderInsert(
    CThostFtdcInputOrderField* input_order,
    CThostFtdcRspInfoField* response_info, int32_t request_id, bool is_last) {
  if (input_order == nullptr) {
    LOG(ERROR) << "Ctp future order client <" << config_.name()
               << "> input order is nullptr";
    return;
  }

  if (response_info != nullptr && response_info->ErrorID != 0) {
    LOG(ERROR) << "Order insert for <" << config_.name()
               << "> failed with error id <" << response_info->ErrorID
               << "> error message <" << response_info->ErrorMsg << ">";

    int64_t order_id = -1;

    try {
      order_id = atol(input_order->OrderRef);
    } catch (const exception& e) {
      LOG(ERROR) << "Ctp future order client <" << config_.name()
                 << "> fail to parse order id <" << input_order->OrderRef
                 << ">";
      return;
    }

    AccountOperator* account_operator =
        account_pool_->GetAccountOperatorByOrderId(order_id);

    if (account_operator == nullptr) {
      LOG(ERROR) << "Account operator does not exist for order <" << order_id
                 << ">";
    } else {
      account_operator->Lock();

      account_operator->UpdateErrorOrder(order_id);

      StrategyMessage strategy_message;
      strategy_message.set_strategy_name(account_operator->GetStrategyName());
      strategy_message.set_type(ORDER);

      strategy_message_queue_->Insert(strategy_message);
      strategy_message_queue_->Notify();

      account_operator->Unlock();
    }
  }
}

void CtpFutureOrderClient::OnErrRtnOrderInsert(
    CThostFtdcInputOrderField* input_order,
    CThostFtdcRspInfoField* response_info) {
  if (input_order == nullptr) {
    LOG(ERROR) << "Ctp future order client <" << config_.name()
               << "> input order is nullptr";
    return;
  }

  if (response_info != nullptr && response_info->ErrorID != 0) {
    LOG(ERROR) << "Order insert for <" << config_.name()
               << "> failed with error id <" << response_info->ErrorID
               << "> error message <" << response_info->ErrorMsg << ">";

    int64_t order_id = -1;

    try {
      order_id = atol(input_order->OrderRef);
    } catch (const exception& e) {
      LOG(ERROR) << "Ctp future order client <" << config_.name()
                 << "> fail to parse order id <" << input_order->OrderRef
                 << ">";
      return;
    }

    AccountOperator* account_operator =
        account_pool_->GetAccountOperatorByOrderId(order_id);

    if (account_operator == nullptr) {
      LOG(ERROR) << "Account operator does not exist for order <" << order_id
                 << ">";
    } else {
      account_operator->Lock();

      account_operator->UpdateErrorOrder(order_id);

      StrategyMessage strategy_message;
      strategy_message.set_strategy_name(account_operator->GetStrategyName());
      strategy_message.set_type(ORDER);

      strategy_message_queue_->Insert(strategy_message);
      strategy_message_queue_->Notify();

      account_operator->Unlock();
    }
  }
}

void CtpFutureOrderClient::OnRtnOrder(CThostFtdcOrderField* order) {
  if (order == nullptr) {
    LOG(ERROR) << "Ctp future order client <" << config_.name()
               << "> order is nullptr";
    return;
  }

  int64_t order_id = -1;

  try {
    order_id = atol(order->OrderRef);
  } catch (const exception& e) {
    LOG(ERROR) << "Ctp future order client <" << config_.name()
               << "> fail to parse order id <" << order->OrderRef << ">";
    return;
  }

  AccountOperator* account_operator =
      account_pool_->GetAccountOperatorByOrderId(order_id);

  if (account_operator == nullptr) {
    LOG(ERROR) << "Account operator does not exist for order <" << order_id
               << ">";
  } else {
    int64_t total_volume = order->VolumeTotalOriginal;
    int64_t traded_volume = order->VolumeTraded;
    int64_t left_volume = order->VolumeTotal;

    LOG(INFO) << "Order <" << order_id << "> order_status <"
              << order->OrderStatus << "> total volume <" << total_volume
              << "> traded_volume <" << traded_volume << "> left_volume <"
              << left_volume << ">";

    account_operator->Lock();

    switch (order->OrderStatus) {
      case THOST_FTDC_OST_Canceled:
        account_operator->UpdateCancelOrder(order_id);
        break;
      case THOST_FTDC_OST_AllTraded:
        account_operator->UpdateTradedOrder(order_id, total_volume,
                                            traded_volume, left_volume);
        break;
      case THOST_FTDC_OST_PartTradedQueueing:
        account_operator->UpdateTradedOrder(order_id, total_volume,
                                            traded_volume, left_volume);
        break;
      default:
        account_operator->UpdateTradedOrder(order_id, total_volume,
                                            traded_volume, left_volume);
        break;
    }
    account_operator->Unlock();
  }
}

void CtpFutureOrderClient::OnRtnTrade(CThostFtdcTradeField* trade) {
  if (trade == nullptr) {
    LOG(ERROR) << "Ctp future order client <" << config_.name()
               << "> trade is nullptr";
    return;
  }

  int64_t order_id = -1;

  try {
    order_id = atol(trade->OrderRef);
  } catch (const exception& e) {
    LOG(ERROR) << "Ctp future order client <" << config_.name()
               << "> fail to parse order id <" << trade->OrderRef << ">";
    return;
  }

  AccountOperator* account_operator =
      account_pool_->GetAccountOperatorByOrderId(order_id);

  if (account_operator == nullptr) {
    LOG(ERROR) << "Account operator does not exist for order <" << order_id
               << ">";
  } else {
    double price = trade->Price;
    int64_t volume = trade->Volume;

    LOG(INFO) << "Confirm order <" << order_id << "> price <" << price
              << "> volume <" << volume << ">";

    account_operator->Lock();

    account_operator->ConfirmTradedOrder(order_id, volume, price);

    StrategyMessage strategy_message;
    strategy_message.set_strategy_name(account_operator->GetStrategyName());
    strategy_message.set_type(ORDER);

    strategy_message_queue_->Insert(strategy_message);
    strategy_message_queue_->Notify();

    account_operator->Unlock();
  }
}

void CtpFutureOrderClient::OnRspOrderAction(
    CThostFtdcInputOrderActionField* input_order_action,
    CThostFtdcRspInfoField* response_info, int32_t request_id, bool is_last) {
  if (input_order_action == nullptr) {
    LOG(INFO) << "Ctp future order client <" << config_.name()
              << "> input_order_action is nullptr";
    return;
  }

  LOG(INFO) << "Order <" << input_order_action->OrderRef << "> action <"
            << input_order_action->ActionFlag << "> with error id <"
            << response_info->ErrorID << "> error message <"
            << response_info->ErrorMsg << ">";
}

void CtpFutureOrderClient::OnRspQryTradingAccount(
    CThostFtdcTradingAccountField* trading_account,
    CThostFtdcRspInfoField* response_info, int32_t request_id, bool is_last) {
  if (response_info != nullptr && response_info->ErrorID != 0) {
    LOG(ERROR) << "Request <" << request_id << "> with error id <"
               << response_info->ErrorID << "> error message <"
               << response_info->ErrorMsg << ">";
    return;
  }

  if (trading_account != nullptr) {
    LOG(INFO) << "broker id: " << trading_account->BrokerID << endl
              << "account id: " << trading_account->AccountID << endl
              << "pre mortgage: " << trading_account->PreMortgage << endl
              << "pre credit: " << trading_account->PreCredit << endl
              << "pre deposit: " << trading_account->PreDeposit << endl
              << "pre balance: " << trading_account->PreBalance << endl
              << "pre margin: " << trading_account->PreMargin << endl
              << "interest base: " << trading_account->InterestBase << endl
              << "interest: " << trading_account->Interest << endl
              << "deposit: " << trading_account->Deposit << endl
              << "withdraw: " << trading_account->Withdraw << endl
              << "frozen margin: " << trading_account->FrozenMargin << endl
              << "frozen cash: " << trading_account->FrozenCash << endl
              << "frozen commission: " << trading_account->FrozenCommission
              << endl
              << "current margin: " << trading_account->CurrMargin << endl
              << "cash in: " << trading_account->CashIn << endl
              << "commission: " << trading_account->Commission << endl
              << "close profit: " << trading_account->CloseProfit << endl
              << "position profit: " << trading_account->PositionProfit << endl
              << "balance: " << trading_account->Balance << endl
              << "available: " << trading_account->Available << endl
              << "withdraw quota: " << trading_account->WithdrawQuota << endl
              << "reserve: " << trading_account->Reserve << endl
              << "trading day: " << trading_account->TradingDay << endl
              << "settlement id: " << trading_account->SettlementID << endl
              << "credit: " << trading_account->Credit << endl
              << "mortgage: " << trading_account->Mortgage << endl
              << "exchange margin: " << trading_account->ExchangeMargin << endl;
  }
}

void CtpFutureOrderClient::OnRspQryInvestorPositionDetail(
    CThostFtdcInvestorPositionDetailField* investor_position_detail,
    CThostFtdcRspInfoField* response_info, int32_t request_id, bool is_last) {
  if (response_info != nullptr && response_info->ErrorID != 0) {
    LOG(ERROR) << "Request <" << request_id << "> with error id <"
               << response_info->ErrorID << "> error message <"
               << response_info->ErrorMsg << ">";
    return;
  }

  if (investor_position_detail != nullptr) {
    LOG(INFO) << "instrument id: " << investor_position_detail->InstrumentID
              << endl
              << " broker id: " << investor_position_detail->BrokerID << endl
              << " investor id: " << investor_position_detail->InvestorID
              << endl
              << " hedge flag: " << investor_position_detail->HedgeFlag << endl
              << " direction: " << investor_position_detail->Direction << endl
              << " open date: " << investor_position_detail->OpenDate << endl
              << " trade id: " << investor_position_detail->TradeID << endl
              << " volume: " << investor_position_detail->Volume << endl
              << " open price: " << investor_position_detail->OpenPrice << endl
              << " trading day: " << investor_position_detail->TradingDay
              << endl
              << " settlement id: " << investor_position_detail->SettlementID
              << endl
              << " trade type: " << investor_position_detail->TradeType << endl
              << " comb instrument id: "
              << investor_position_detail->CombInstrumentID << endl;
  }
}

void CtpFutureOrderClient::OnRspQryInstrument(
    CThostFtdcInstrumentField* instrument,
    CThostFtdcRspInfoField* response_info, int request_id, bool is_last) {
  if (response_info != nullptr && response_info->ErrorID != 0) {
    LOG(ERROR) << "Request <" << request_id << "> with error id <"
               << response_info->ErrorID << "> error message <"
               << response_info->ErrorMsg << ">";
    return;
  }

  if (instrument == nullptr) {
    LOG(ERROR) << "Ctp future order client <" << config_.name()
               << "> instrument is nullptr";
    return;
  }

  Security security;

  security.set_country(kCountry);
  security.set_exchange(instrument->ExchangeID);
  security.set_name(instrument->InstrumentID);
  
  string id = security.country() + "." + security.exchange() + "." +
                  security.name();
  replace(id.begin(), id.end(), ' ', '_');
  replace(id.begin(), id.end(), '&', '_');
  
  security.set_id(id);
  security.set_product(instrument->ProductID);
  security.set_security_type(CTP_FUTURE);
  security.set_multiplier(instrument->VolumeMultiple);
  security.set_tick(instrument->PriceTick);
  security.set_long_margin(instrument->LongMarginRatio);
  security.set_short_margin(instrument->ShortMarginRatio);
  security.set_is_trading(instrument->IsTrading);
  security.set_open_date(instrument->OpenDate);

  if (security.is_trading()) {
    DatabaseOperator database_operator(config_.database_config());

    if (!database_operator.Init()) {
      return;
    }

    string command = "INSERT INTO securities VALUES ('";
    command += security.country() + "','";
    command += security.exchange() + "','";
    command += security.product() + "','";
    command += security.id() + "','";

    TextFormat::Printer printer;
    printer.SetSingleLineMode(true);

    string security_string;
    printer.PrintToString(security, &security_string);

    command += security_string;
    command += "') ON DUPLICATE KEY UPDATE security='";
    command += security_string + "';";

    database_operator.Write(command);

    LOG(INFO) << "Insert security <" << security.name() << ">";
  }
}

void CtpFutureOrderClient::PlaceOrder(const OrderMessage& order_message) {
  string account_id = order_message.account_id();

  AccountOperator* account_operator =
      account_pool_->GetAccountOperatorByAccountId(account_id);

  if (account_operator == nullptr) {
    LOG(WARNING) << "account_operator <" << account_id << "> does not exit";
    return;
  }

  account_operator->Lock();

  int64_t order_id = order_message.order_id();
  if (!account_operator->FoundNewOrder(order_id)) {
    LOG(WARNING) << "New order <" << order_id << "> not found";
    return;
  }

  Account account = account_operator->GetAccount();

  const Order& order = account.order_id_to_order().at(order_id);

  CThostFtdcInputOrderField ctp_order;

  memset(&ctp_order, 0, sizeof(ctp_order));

  strcpy(ctp_order.BrokerID, config_.broker_id().c_str());
  strcpy(ctp_order.InvestorID, config_.user_id().c_str());
  strcpy(ctp_order.UserID, config_.user_id().c_str());
  strcpy(ctp_order.InstrumentID, order.name().c_str());
  strcpy(ctp_order.OrderRef, to_string(order_id).c_str());

  switch (order.action()) {
    case BUY:
      ctp_order.Direction = THOST_FTDC_D_Buy;
      break;
    case SELL:
      ctp_order.Direction = THOST_FTDC_D_Sell;
      break;
    default:
      break;
  }

  switch (order.direction()) {
    case OPEN:
      ctp_order.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
      break;
    case CLOSE:
      ctp_order.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
      break;
    case CLOSE_TODAY:
      ctp_order.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
      break;
    case CLOSE_YESTERDAY:
      ctp_order.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;
      break;
    default:
      break;
  }

  ctp_order.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;

  switch (order.price_type()) {
    case LIMIT_PRICE:
      ctp_order.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
      ctp_order.LimitPrice = order.price();
      break;
    case MARKET_PRICE:
      ctp_order.OrderPriceType = THOST_FTDC_OPT_BestPrice;
      break;
    default:
      break;
  }

  ctp_order.VolumeTotalOriginal = order.quantity();
  ctp_order.TimeCondition = THOST_FTDC_TC_GFD;
  ctp_order.VolumeCondition = THOST_FTDC_VC_AV;
  ctp_order.MinVolume = 1;
  ctp_order.ContingentCondition = THOST_FTDC_CC_Immediately;
  ctp_order.StopPrice = 0;
  ctp_order.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
  ctp_order.IsAutoSuspend = 0;

  if (trader_api_->ReqOrderInsert(&ctp_order, request_id_++) != 0) {
    account_operator->UpdateErrorOrder(order_id);

    StrategyMessage strategy_message;
    strategy_message.set_strategy_name(account_operator->GetStrategyName());
    strategy_message.set_type(ORDER);

    strategy_message_queue_->Insert(strategy_message);
    strategy_message_queue_->Notify();
  } else {
    account_operator->UpdateOrderStatus(order_id, ORDER_SENT_TO_EXCHANGE);
  }

  account_operator->Unlock();
}

void CtpFutureOrderClient::CancelOrder(const OrderMessage& order_message) {
  int64_t order_id = order_message.order_id();

  AccountOperator* account_operator =
      account_pool_->GetAccountOperatorByOrderId(order_id);

  if (account_operator == nullptr) {
    LOG(ERROR) << "account_operator for order <" << order_id
               << "> does not exit";
    return;
  }

  account_operator->Lock();

  if (!account_operator->FoundOrder(order_id)) {
    LOG(ERROR) << "Order <" << order_id << "> not found";
    return;
  }

  Account account = account_operator->GetAccount();
  const Order& order = account.order_id_to_order().at(order_id);

  CThostFtdcInputOrderActionField ctp_order;

  memset(&ctp_order, 0, sizeof(ctp_order));

  strcpy(ctp_order.BrokerID, config_.broker_id().c_str());
  strcpy(ctp_order.InvestorID, config_.user_id().c_str());
  strcpy(ctp_order.InstrumentID, order.name().c_str());
  strcpy(ctp_order.OrderRef, to_string(order_id).c_str());

  ctp_order.FrontID = front_id_;
  ctp_order.SessionID = session_id_;
  ctp_order.ActionFlag = THOST_FTDC_AF_Delete;

  if (trader_api_->ReqOrderAction(&ctp_order, request_id_++) != 0) {
    LOG(ERROR) << "Ctp future order client failed to send cancel order <"
               << order_id << ">";
  } else {
    account_operator->UpdateOrderStatus(order_message.order_id(),
                                        CANCEL_SENT_TO_EXCHANGE);
  }

  account_operator->Unlock();
}

void CtpFutureOrderClient::RequestSecurityDetail(
    const OrderMessage& order_message) {
  CThostFtdcQryInstrumentField request;

  memset(&request, 0, sizeof(request));

  if (order_message.has_exchange()) {
    strcpy(request.ExchangeID, order_message.exchange().c_str());
  }

  try {
    if (trader_api_->ReqQryInstrument(&request, request_id_++) != 0) {
      LOG(ERROR) << "Ctp future order client <" << config_.name()
                 << "> failed to request instrument";
    } else {
      LOG(INFO) << "Ctp future order client <" << config_.name()
                << "> requests instrument";
    }
  } catch (const exception& e) {
    LOG(ERROR) << "Ctp future order client <" << config_.name()
               << "> request instrument exception: " << e.what();
  }
}

void CtpFutureOrderClient::RequestAccountInfo() {
  CThostFtdcQryTradingAccountField request;

  memset(&request, 0, sizeof(request));

  strcpy(request.BrokerID, config_.broker_id().c_str());
  strcpy(request.InvestorID, config_.user_id().c_str());

  try {
    if (trader_api_->ReqQryTradingAccount(&request, request_id_++) != 0) {
      LOG(ERROR) << "Ctp future order client >" << config_.name()
                 << "> failed to request account info";
    } else {
      LOG(INFO) << "Ctp future order client <" << config_.name()
                << "> requests account info";
    }
  } catch (const exception& e) {
    LOG(FATAL) << "Ctp future order client " << config_.name()
               << " request account info exception: " << e.what();
    exit(1);
  }
}

void CtpFutureOrderClient::RequestAccountDetail() {
  CThostFtdcQryInvestorPositionDetailField request;

  memset(&request, 0, sizeof(request));
  strcpy(request.BrokerID, config_.broker_id().c_str());
  strcpy(request.InvestorID, config_.user_id().c_str());

  try {
    if (trader_api_->ReqQryInvestorPositionDetail(&request, request_id_++) !=
        0) {
      LOG(ERROR) << "Ctp future order client <" << config_.name()
                 << "> failed to request account detail";
    } else {
      LOG(INFO) << "Ctp future order client <" << config_.name()
                << "> requests account detail";
    }
  } catch (const exception& e) {
    LOG(FATAL) << "Ctp future order client <" << config_.name()
               << "> request account detail exception: " << e.what();
    exit(1);
  }
}

bool CtpFutureOrderClient::IsInTradingHours() const {
  for (const auto& time_interval : config_.time_interval()) {
    time_duration begin_time = duration_from_string(time_interval.begin_time());
    time_duration end_time = duration_from_string(time_interval.end_time());

    time_duration now = second_clock::local_time().time_of_day();
    if (now >= begin_time && now <= end_time) {
      return true;
    }
  }

  return false;
}

}  // namespace order_maker
