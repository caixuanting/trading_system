// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "modules/data_receiver/clients/ctp_future_data_client.h"

#include <exception>
#include <string>
#include <thread>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

#include <glog/logging.h>

#include "utils/date_time.h"

using boost::algorithm::join;
using boost::filesystem::exists;
using boost::filesystem::path;
using boost::filesystem::system_complete;
using boost::gregorian::date;
using boost::gregorian::date;
using boost::gregorian::day_clock;
using boost::gregorian::day_clock;
using boost::posix_time::duration_from_string;
using boost::posix_time::microsec_clock;
using boost::posix_time::ptime;
using boost::posix_time::second_clock;
using boost::posix_time::seconds;
using boost::posix_time::time_duration;
using boost::posix_time::time_from_string;
using boost::posix_time::to_iso_string;
using boost::posix_time::to_simple_string;
using event_scheduler::Action;
using event_scheduler::Event;
using interface::CTP_FUTURE;
using interface::Data;
using interface::DataPool;
using interface::Security;
using interface::Snapshot;
using std::endl;
using std::exception;
using std::move;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::vector;
using util::Queue;
using util::ToString;

namespace data_receiver {
namespace {

// 24 hours.
const int32_t kSecondsPerDay = 24 * 3600;
// 1 minute.
const int32_t kTimeToWaitInSeconds = 60;
// 1 billion.
const int64_t kMaxNum = 1000000000;
// 5 seconds.
const int32_t kLateQuotaInSeconds = 5;

}  // namespace

CtpFutureDataClient::CtpFutureDataClient(
    const CtpFutureDataClientConfig& config, DataPool* data_pool,
    Queue<Event*>* event_queue, Queue<DataMessage>* data_message_queue)
    : DataClient(data_pool),
      config_(config),
      data_message_queue_(data_message_queue),
      event_queue_(event_queue) {
  // When client returns data, it only returns name but not id.
  // So we need keep track of name to id map by ourselves.
  for (const auto& id_to_data : *data_pool) {
    Data* data = id_to_data.second.get();

    if (data == nullptr) {
      LOG(ERROR) << "Data is nullptr for <" << id_to_data.first << ">";
      continue;
    }

    const Security& security = data->GetSecurity();

    if (security.security_type() == CTP_FUTURE &&
        (config_.exchange() == "" ||
         security.exchange() == config_.exchange())) {
      name_to_id_[security.name()] = security.id();
    }
  }
}

CtpFutureDataClient::~CtpFutureDataClient() {}

void CtpFutureDataClient::Init() {
  LOG(INFO) << "Initing client <" << config_.name() << ">";

  try {
    path complete_path(system_complete(config_.log_dir()));
    string complete_path_string = complete_path.string();

    if (!exists(complete_path_string)) {
      string command = "mkdir -p " + complete_path_string;
      LOG(INFO) << "Creating directory " << complete_path_string
                << " status: " << system(command.c_str());
    }

    user_api_ = CThostFtdcMdApi::CreateFtdcMdApi(complete_path_string.c_str());

    LOG(INFO) << "Ctp future data client <" << config_.name()
              << "> is using Version <" << user_api_->GetApiVersion()
              << "> api";
  } catch (const exception& e) {
    LOG(FATAL) << "Ctp future data client <" << config_.name()
               << "> fails to create User Api: " << e.what();

    exit(1);
  }

  date today = day_clock::local_day();

  for (const auto& time_interval : config_.time_interval()) {
    // kTimeToWaitInSeconds seconds before the market opens.
    time_duration begin_time =
        duration_from_string(time_interval.begin_time()) -
        seconds(kTimeToWaitInSeconds);
    ptime begin_start_time(today, begin_time);

    // Prepare user login event.
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

    // Schedule "user login" event.
    event_queue_->Insert(events_.back().get());
    event_queue_->Notify();

    // kTimeToWaitInSeconds seconds after the market closes.
    time_duration end_time = duration_from_string(time_interval.end_time()) +
                             seconds(kTimeToWaitInSeconds);
    ptime end_start_time(today, end_time);

    // Prepare unsbuscribe data event.
    Action end_action = [this, end_time] {
      time_duration now = second_clock::local_time().time_of_day();

      if (now > end_time + seconds(kLateQuotaInSeconds)) {
        return;
      }

      string end_time_string = to_simple_string(end_time);

      LOG(INFO) << "Start unsubscribing data at <" << end_time_string << ">";

      this->UnSubscribeData();

      LOG(INFO) << "Finished unsubscribing data at <" << end_time_string << ">";
    };

    unique_ptr<Event> end_event(new Event(
        end_action, end_start_time, kSecondsPerDay,
        "Unsubscribe data every day at " + to_simple_string(end_time)));
    events_.emplace_back(move(end_event));

    // Schedule "unsubscribe data" event.
    event_queue_->Insert(events_.back().get());
    event_queue_->Notify();
  }

  LOG(INFO) << "Inited client <" << config_.name() << ">";
}

void CtpFutureDataClient::Start() {
  LOG(INFO) << "Starting client <" << config_.name() << ">";

  if (user_api_ == nullptr) {
    LOG(FATAL) << "User api is nullptr for client <" << config_.name() << ">";

    exit(1);
  }

  try {
    user_api_->RegisterSpi(this);
    for (const string& frontend_address : config_.frontend_address()) {
      user_api_->RegisterFront(const_cast<char*>(frontend_address.c_str()));
    }
    user_api_->Init();
    user_api_->Join();
  } catch (const exception& e) {
    LOG(FATAL) << "Fail to start User Api: " << e.what();

    exit(1);
  }

  LOG(INFO) << "Started client <" << config_.name() << ">";
}

void CtpFutureDataClient::Exit() {
  LOG(INFO) << "Exiting client <" << config_.name() << ">";

  // Stop user api.
  try {
    user_api_->RegisterSpi(nullptr);
    user_api_->Release();
    user_api_ = nullptr;
  } catch (const exception& e) {
    LOG(FATAL) << "Fail to release User Api: " << e.what();

    exit(1);
  }

  // Cancel user login and unsubscribe data events.
  for (const auto& event : events_) {
    event->Cancel();
  }

  LOG(INFO) << "Exited client <" << config_.name() << ">";
}

void CtpFutureDataClient::UserLogin() {
  CThostFtdcReqUserLoginField request;

  memset(&request, 0, sizeof(request));
  strcpy(request.BrokerID, config_.broker_id().c_str());
  strcpy(request.UserID, config_.user_id().c_str());
  strcpy(request.Password, config_.password().c_str());

  int32_t request_id = request_id_++;

  if (user_api_->ReqUserLogin(&request, request_id) != 0) {
    LOG(ERROR) << "User login request <" << request_id << "> for <"
               << config_.name() << "> failed";

  } else {
    LOG(INFO) << "Request <" << request_id << "> for user login is sent";
  }
}

void CtpFutureDataClient::SubscribeData() {
  vector<string> symbols;

  for (const auto& id_to_data : *data_pool_) {
    Data* data = id_to_data.second.get();

    if (data == nullptr) {
      LOG(ERROR) << "Data is nullptr for <" << id_to_data.first << ">";
      continue;
    }

    const Security& security = data->GetSecurity();

    if (security.security_type() == CTP_FUTURE &&
        (config_.exchange() == "" ||
         security.exchange() == config_.exchange())) {
      symbols.push_back(security.name());
    }
  }

  size_t size = symbols.size();

  LOG(INFO) << "Subscribe <" << size << "> securities";

  char** instruments = new char*[size];

  for (uint32_t i = 0; i < size; i++) {
    instruments[i] = const_cast<char*>(symbols[i].c_str());
  }

  try {
    user_api_->SubscribeMarketData(instruments, size);
  } catch (const exception& e) {
    LOG(ERROR) << "Fail to subscribe data " << e.what();
  }

  delete instruments;

  LOG(INFO) << "Subscribe data for <" << join(symbols, ",") << ">";
}

void CtpFutureDataClient::UnSubscribeData() {
  vector<string> symbols;

  for (const auto& id_to_data : *data_pool_) {
    Data* data = id_to_data.second.get();

    if (data == nullptr) {
      LOG(ERROR) << "Data is nullptr for <" << id_to_data.first << ">";
      continue;
    }

    const Security& security = data->GetSecurity();

    if (security.security_type() == CTP_FUTURE) {
      symbols.push_back(security.name());
    }
  }

  size_t size = symbols.size();

  char** instruments = new char*[size];

  for (uint32_t i = 0; i < size; i++) {
    instruments[i] = const_cast<char*>(symbols[i].c_str());
  }

  try {
    user_api_->UnSubscribeMarketData(instruments, size);
  } catch (const exception& e) {
    LOG(ERROR) << "Fail to unsubscribe data: " << e.what();
  }

  delete instruments;

  LOG(INFO) << "UnSubscribe data for <" << join(symbols, ",") << ">";
}

void CtpFutureDataClient::OnFrontConnected() {
  LOG_EVERY_N(INFO, 100) << "Ctp future data client <" << config_.name() << "> connected";

  // We schedule UserLogin before market open.
  // But when the connection is broken during trading hours,
  // we need login again.
  if (IsInTradingHours()) {
    UserLogin();
  }
}

void CtpFutureDataClient::OnFrontDisconnected(int32_t reason) {
  LOG_EVERY_N(INFO, 1000) << "Ctp future data client <" << config_.name() << "> disconnected";
}

void CtpFutureDataClient::OnRspError(CThostFtdcRspInfoField* response_info,
                                     int32_t request_id, bool is_last) {
  if (response_info != nullptr && response_info->ErrorID != 0) {
    LOG(WARNING) << "Error for request <" << request_id << "> with error id <"
                 << response_info->ErrorID << "> error message <"
                 << response_info->ErrorMsg << ">";
  } else {
    LOG(INFO) << "No error for request " << request_id;
  }
}

void CtpFutureDataClient::OnRspUserLogin(
    CThostFtdcRspUserLoginField* response_user_login,
    CThostFtdcRspInfoField* response_info, int32_t request_id, bool is_last) {
  if ((response_info != nullptr && response_info->ErrorID != 0)) {
    LOG(WARNING) << "User login failed with error id <"
                 << response_info->ErrorID << "> error message <"
                 << response_info->ErrorMsg << ">";

    return;
  }

  if (response_user_login == nullptr) {
    LOG(WARNING) << "response_user_login is nullptr";

    return;
  }

  front_id_ = response_user_login->FrontID;
  session_id_ = response_user_login->SessionID;

  LOG(INFO) << "Ctp future data client <" << config_.name()
            << "> user login successful " << endl
            << "trading day <" << response_user_login->TradingDay << ">" << endl
            << "login time <" << response_user_login->LoginTime << ">" << endl
            << "broker id <" << response_user_login->BrokerID << ">" << endl
            << "user id <" << response_user_login->UserID << ">" << endl
            << "system name <" << response_user_login->SystemName << ">" << endl
            << "front id <" << response_user_login->FrontID << ">" << endl
            << "session id <" << response_user_login->SessionID << ">" << endl
            << "max order ref <" << response_user_login->MaxOrderRef << ">"
            << endl
            << "SHFE time <" << response_user_login->SHFETime << ">" << endl
            << "DCE time <" << response_user_login->DCETime << ">" << endl
            << "CZCE time <" << response_user_login->CZCETime << ">" << endl
            << "FFXE time <" << response_user_login->FFEXTime << ">" << endl
            << "INE time <" << response_user_login->INETime << ">";

  SubscribeData();
}

void CtpFutureDataClient::OnRspSubMarketData(
    CThostFtdcSpecificInstrumentField* specific_instrument,
    CThostFtdcRspInfoField* response_info, int32_t request_id, bool is_last) {
  if (specific_instrument == nullptr) {
    LOG(WARNING) << "specific_instrument is nullptr";

    return;
  }

  if (response_info != nullptr && response_info->ErrorID != 0) {
    LOG(WARNING) << "Instrument <" << specific_instrument->InstrumentID
                 << "> subscription failed with error id <"
                 << response_info->ErrorID << "> error message <"
                 << response_info->ErrorMsg << ">";
  } else {
    LOG(INFO) << "Instrument <" << specific_instrument->InstrumentID
              << "> subscribed";
  }
}

void CtpFutureDataClient::OnRspUnSubMarketData(
    CThostFtdcSpecificInstrumentField* specific_instrument,
    CThostFtdcRspInfoField* response_info, int32_t request_id, bool is_last) {
  if (specific_instrument == nullptr) {
    LOG(WARNING) << "specific_instrument is nullptr";

    return;
  }

  if (response_info != nullptr && response_info->ErrorID != 0) {
    LOG(ERROR) << "Instrument <" << specific_instrument->InstrumentID
               << "> unsubscription failed with error id <"
               << response_info->ErrorID << "> error message <"
               << response_info->ErrorMsg << ">";
  } else {
    LOG(INFO) << "Instrument <" << specific_instrument->InstrumentID
              << "> unsubscribed";
  }
}

void CtpFutureDataClient::OnRtnDepthMarketData(
    CThostFtdcDepthMarketDataField* depth_market_data) {
  if (depth_market_data == nullptr) {
    LOG(WARNING) << "depth_market_data is nullptr";

    return;
  }

  Snapshot snapshot;

  snapshot.set_instrument_id(depth_market_data->InstrumentID);
  snapshot.set_exchange_id(depth_market_data->ExchangeID);
  snapshot.set_exchange_inst_id(depth_market_data->ExchangeInstID);

  if (depth_market_data->LastPrice < kMaxNum) {
    snapshot.set_last_price(depth_market_data->LastPrice);
  }

  if (depth_market_data->PreSettlementPrice < kMaxNum) {
    snapshot.set_pre_settlement_price(depth_market_data->PreSettlementPrice);
  }

  if (depth_market_data->PreClosePrice < kMaxNum) {
    snapshot.set_pre_close_price(depth_market_data->PreClosePrice);
  }

  if (depth_market_data->PreOpenInterest < kMaxNum) {
    snapshot.set_pre_open_interest(depth_market_data->PreOpenInterest);
  }

  if (depth_market_data->OpenPrice < kMaxNum) {
    snapshot.set_open_price(depth_market_data->OpenPrice);
  }

  if (depth_market_data->HighestPrice < kMaxNum) {
    snapshot.set_highest_price(depth_market_data->HighestPrice);
  }

  if (depth_market_data->LowestPrice < kMaxNum) {
    snapshot.set_lowest_price(depth_market_data->LowestPrice);
  }

  if (depth_market_data->Volume < kMaxNum) {
    snapshot.set_total_volume(depth_market_data->Volume);
  }

  if (depth_market_data->Turnover < kMaxNum) {
    snapshot.set_turn_over(depth_market_data->Turnover);
  }

  if (depth_market_data->OpenInterest < kMaxNum) {
    snapshot.set_open_interest(depth_market_data->OpenInterest);
  }

  if (depth_market_data->ClosePrice < kMaxNum) {
    snapshot.set_close_price(depth_market_data->ClosePrice);
  }

  if (depth_market_data->SettlementPrice < kMaxNum) {
    snapshot.set_settlement_price(depth_market_data->SettlementPrice);
  }

  if (depth_market_data->UpperLimitPrice < kMaxNum) {
    snapshot.set_upper_limit_price(depth_market_data->UpperLimitPrice);
  }

  if (depth_market_data->LowerLimitPrice < kMaxNum) {
    snapshot.set_lower_limit_price(depth_market_data->LowerLimitPrice);
  }

  if (depth_market_data->PreDelta < kMaxNum) {
    snapshot.set_pre_delta(depth_market_data->PreDelta);
  }

  if (depth_market_data->CurrDelta < kMaxNum) {
    snapshot.set_curr_delta(depth_market_data->CurrDelta);
  }

  if (depth_market_data->BidPrice1 < kMaxNum) {
    snapshot.set_level_1_bid_price(depth_market_data->BidPrice1);
  }

  if (depth_market_data->BidPrice2 < kMaxNum) {
    snapshot.set_level_2_bid_price(depth_market_data->BidPrice2);
  }

  if (depth_market_data->BidPrice3 < kMaxNum) {
    snapshot.set_level_3_bid_price(depth_market_data->BidPrice3);
  }

  if (depth_market_data->BidPrice4 < kMaxNum) {
    snapshot.set_level_4_bid_price(depth_market_data->BidPrice4);
  }

  if (depth_market_data->BidPrice5 < kMaxNum) {
    snapshot.set_level_5_bid_price(depth_market_data->BidPrice5);
  }

  if (depth_market_data->BidVolume1 < kMaxNum) {
    snapshot.set_level_1_bid_volume(depth_market_data->BidVolume1);
  }

  if (depth_market_data->BidVolume2 < kMaxNum) {
    snapshot.set_level_2_bid_volume(depth_market_data->BidVolume2);
  }

  if (depth_market_data->BidVolume3 < kMaxNum) {
    snapshot.set_level_3_bid_volume(depth_market_data->BidVolume3);
  }

  if (depth_market_data->BidVolume4 < kMaxNum) {
    snapshot.set_level_4_bid_volume(depth_market_data->BidVolume4);
  }

  if (depth_market_data->BidVolume5 < kMaxNum) {
    snapshot.set_level_5_bid_volume(depth_market_data->BidVolume5);
  }

  if (depth_market_data->AskPrice1 < kMaxNum) {
    snapshot.set_level_1_ask_price(depth_market_data->AskPrice1);
  }

  if (depth_market_data->AskPrice2 < kMaxNum) {
    snapshot.set_level_2_ask_price(depth_market_data->AskPrice2);
  }

  if (depth_market_data->AskPrice3 < kMaxNum) {
    snapshot.set_level_3_ask_price(depth_market_data->AskPrice3);
  }

  if (depth_market_data->AskPrice4 < kMaxNum) {
    snapshot.set_level_4_ask_price(depth_market_data->AskPrice4);
  }

  if (depth_market_data->AskPrice5 < kMaxNum) {
    snapshot.set_level_5_ask_price(depth_market_data->AskPrice5);
  }

  if (depth_market_data->AskVolume1 < kMaxNum) {
    snapshot.set_level_1_ask_volume(depth_market_data->AskVolume1);
  }

  if (depth_market_data->AskVolume2 < kMaxNum) {
    snapshot.set_level_2_ask_volume(depth_market_data->AskVolume2);
  }

  if (depth_market_data->AskVolume3 < kMaxNum) {
    snapshot.set_level_3_ask_volume(depth_market_data->AskVolume3);
  }

  if (depth_market_data->AskVolume4 < kMaxNum) {
    snapshot.set_level_4_ask_volume(depth_market_data->AskVolume4);
  }

  if (depth_market_data->AskVolume5 < kMaxNum) {
    snapshot.set_level_5_ask_volume(depth_market_data->AskVolume5);
  }

  if (depth_market_data->AveragePrice < kMaxNum) {
    snapshot.set_average_price(depth_market_data->AveragePrice);
  }

	string action_day = depth_market_data->ActionDay;
	
	if (action_day.empty()) {
		date today = day_clock::local_day();
		action_day = to_iso_string(today);
	}
	
  snapshot.set_action_day(action_day);
  snapshot.set_trading_day(depth_market_data->TradingDay);

  string exchange_time = action_day + string(" ") +
                         depth_market_data->UpdateTime + string(".") +
                         to_string(depth_market_data->UpdateMillisec);
  exchange_time.insert(4, "-");
  exchange_time.insert(7, "-");

  snapshot.set_exchange_time(exchange_time);
  snapshot.set_local_time(ToString(microsec_clock::local_time()));
  snapshot.set_is_valid(true);

  data_pool_->InsertSnapshot(name_to_id_[snapshot.instrument_id()], snapshot);

  DataMessage data_message;

  data_message.set_data_id(name_to_id_[snapshot.instrument_id()]);
  data_message.set_timestamp(ToString(microsec_clock::local_time()));

  // Notify system that we have new data.
  data_message_queue_->Insert(data_message);
  data_message_queue_->Notify();
}

bool CtpFutureDataClient::IsInTradingHours() const {
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

}  // namespace data_receiver
