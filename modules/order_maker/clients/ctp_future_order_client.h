// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef MODULES_ORDER_MAKER_CLIENTS_CTP_FUTURE_ORDER_MAKER_H_
#define MODULES_ORDER_MAKER_CLIENTS_CTP_FUTURE_ORDER_MAKER_H_

#include <thread>

#include <ctp/ThostFtdcTraderApi.h>

#include "interfaces/account/account_pool.h"
#include "modules/event_scheduler/event.h"
#include "modules/order_maker/clients/order_client.h"
#include "modules/order_maker/clients/proto/ctp_future_order_client_config.pb.h"
#include "modules/order_maker/proto/order_message.pb.h"
#include "modules/strategy_runner/proto/strategy_message.pb.h"
#include "utils/queue.h"

namespace order_maker {

class CtpFutureOrderClient : public CThostFtdcTraderSpi, public OrderClient {
 public:
  CtpFutureOrderClient(
      const CtpFutureOrderClientConfig& config,
      interface::AccountPool* account_pool,
      util::Queue<event_scheduler::Event*>* event_queue,
      util::Queue<OrderMessage>* order_message_queue,
      util::Queue<strategy_runner::StrategyMessage>* strategy_message_queue);
  virtual ~CtpFutureOrderClient();

  void Init() override;
  void Start() override;
  void Exit() override;

  void UserLogin();
  void SettlementConfirm();

  void OnFrontConnected() override;
  void OnFrontDisconnected(int32_t reason) override;
  void OnRspError(CThostFtdcRspInfoField* response_info, int32_t request_id,
                  bool is_last) override;
  void OnRspUserLogin(CThostFtdcRspUserLoginField* response_user_login,
                      CThostFtdcRspInfoField* response_info, int32_t request_id,
                      bool is_last) override;
  void OnRspSettlementInfoConfirm(
      CThostFtdcSettlementInfoConfirmField* settlement_info_confirm,
      CThostFtdcRspInfoField* response_info, int32_t request_id,
      bool is_last) override;
  void OnRspOrderInsert(CThostFtdcInputOrderField* input_order,
                        CThostFtdcRspInfoField* response_info,
                        int32_t request_id, bool is_last) override;
  void OnErrRtnOrderInsert(CThostFtdcInputOrderField* input_order,
                           CThostFtdcRspInfoField* response_info) override;
  void OnRtnOrder(CThostFtdcOrderField* order) override;
  void OnRtnTrade(CThostFtdcTradeField* trade) override;
  void OnRspOrderAction(CThostFtdcInputOrderActionField* input_order_action,
                        CThostFtdcRspInfoField* response_info,
                        int32_t request_id, bool is_last) override;
  void OnRspQryTradingAccount(CThostFtdcTradingAccountField* trading_account,
                              CThostFtdcRspInfoField* response_info,
                              int32_t request_id, bool is_last) override;
  void OnRspQryInvestorPositionDetail(
      CThostFtdcInvestorPositionDetailField* investor_position_detail,
      CThostFtdcRspInfoField* response_info, int32_t request_id,
      bool is_last) override;
  void OnRspQryInstrument(CThostFtdcInstrumentField* instrument,
                          CThostFtdcRspInfoField* response_info, int request_id,
                          bool is_last);

 private:
  void PlaceOrder(const OrderMessage& order_message);
  void CancelOrder(const OrderMessage& order_message);
  void RequestSecurityDetail(const OrderMessage& order_message);
  void RequestAccountInfo();
  void RequestAccountDetail();
  bool IsInTradingHours() const;

  CThostFtdcTraderApi* trader_api_;  // Not owned.
  CtpFutureOrderClientConfig config_;
  bool running_ = true;
  int32_t front_id_ = -1;
  int32_t request_id_ = 0;
  int32_t session_id_ = -1;
  interface::AccountPool* account_pool_;  // Not owned.
  std::thread api_thread_;
  std::thread message_thread_;
  std::vector<std::unique_ptr<event_scheduler::Event>> events_;
  util::Queue<OrderMessage>* order_message_queue_;     // Not owned.
  util::Queue<event_scheduler::Event*>* event_queue_;  // Not owned.
  util::Queue<strategy_runner::StrategyMessage>*
      strategy_message_queue_;  // Not owned.
};

}  // namespace order_maker

#endif  // MODULES_ORDER_MAKER_CLIENTS_CTP_FUTURE_ORDER_MAKER_H_