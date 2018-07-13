// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef MODUELS_DATA_RECEIVER_CTP_FUTURE_DATA_CLIENT_H_
#define MODUELS_DATA_RECEIVER_CTP_FUTURE_DATA_CLIENT_H_

#include <ctp/ThostFtdcMdApi.h>

#include "modules/data_receiver/clients/data_client.h"
#include "modules/data_receiver/clients/proto/ctp_future_data_client_config.pb.h"
#include "modules/data_receiver/proto/data_message.pb.h"
#include "modules/event_scheduler/event.h"
#include "utils/queue.h"

namespace data_receiver {

typedef std::unordered_map<std::string, std::string> NameToId;

class CtpFutureDataClient : public CThostFtdcMdSpi, public DataClient {
 public:
  CtpFutureDataClient(const CtpFutureDataClientConfig& config,
                      interface::DataPool* data_pool,
                      util::Queue<event_scheduler::Event*>* event_queue,
                      util::Queue<DataMessage>* data_message_queue);
  virtual ~CtpFutureDataClient();

  // Schedule user login and data unsubscribe events.
  void Init() override;
  void Start() override;
  void Exit() override;

  // If we don't log in, no data would come.
  void UserLogin();
  void SubscribeData();
  void UnSubscribeData();

  // API callback.
  void OnFrontConnected() override;
  void OnFrontDisconnected(int32_t reason) override;
  void OnRspError(CThostFtdcRspInfoField* response_info, int32_t request_id,
                  bool is_last) override;
  void OnRspUserLogin(CThostFtdcRspUserLoginField* response_user_login,
                      CThostFtdcRspInfoField* response_info, int32_t request_id,
                      bool is_last) override;
  void OnRspSubMarketData(
      CThostFtdcSpecificInstrumentField* specific_instrument,
      CThostFtdcRspInfoField* response_info, int32_t request_id,
      bool is_last) override;
  void OnRspUnSubMarketData(
      CThostFtdcSpecificInstrumentField* specific_instrument,
      CThostFtdcRspInfoField* response_info, int32_t request_id,
      bool is_last) override;
  void OnRtnDepthMarketData(
      CThostFtdcDepthMarketDataField* depth_market_data) override;

 private:
  // Check whether in trading hours now.
  bool IsInTradingHours() const;

  CThostFtdcMdApi* user_api_ = nullptr;
  CtpFutureDataClientConfig config_;
  int32_t front_id_ = -1;
  int32_t request_id_ = 0;
  int32_t session_id_ = -1;
  NameToId name_to_id_;
  std::vector<std::unique_ptr<event_scheduler::Event>> events_;
  util::Queue<DataMessage>* data_message_queue_;
  util::Queue<event_scheduler::Event*>* event_queue_;
};

}  // namespace data_receiver

#endif  // MODUELS_DATA_RECEIVER_CTP_FUTURE_DATA_CLIENT_H_
