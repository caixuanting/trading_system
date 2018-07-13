/////////////////////////////////////////////////////////////////////////
///@system ��һ������ϵͳ
///@company �Ϻ��ڻ���Ϣ�������޹�˾
///@file ThostFtdcTraderApi.h
///@brief �����˿ͻ��˽ӿ�
///@history
/// 20060106	�Ժ��		�������ļ�
/////////////////////////////////////////////////////////////////////////

#if !defined(THOST_FTDCTRADERAPI_H)
#define THOST_FTDCTRADERAPI_H

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "ThostFtdcUserApiStruct.h"

#if defined(ISLIB) && defined(WIN32)
#ifdef LIB_TRADER_API_EXPORT
#define TRADER_API_EXPORT __declspec(dllexport)
#else
#define TRADER_API_EXPORT __declspec(dllimport)
#endif
#else
#define TRADER_API_EXPORT
#endif

class CThostFtdcTraderSpi {
 public:
  ///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�

  virtual void OnFrontConnected(){};

  ///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ������?
  ///@param nReason ����ԭ��
  ///        0x1001 �����ʧ��
  ///        0x1002 ����дʧ��
  ///        0x2001 ��������ʱ
  ///        0x2002 ��������ʧ��
  ///        0x2003 �յ�������

  virtual void OnFrontDisconnected(int nReason){};

  ///����ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
  ///@param nTimeLapse �����ϴν��ձ��ĵ�ʱ��

  virtual void OnHeartBeatWarning(int nTimeLapse){};

  ///�ͻ�����֤��Ӧ

  virtual void OnRspAuthenticate(
      CThostFtdcRspAuthenticateField *pRspAuthenticateField,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///��¼������Ӧ

  virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast){};

  ///�ǳ�������Ӧ

  virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast){};

  ///�û��������������Ӧ

  virtual void OnRspUserPasswordUpdate(
      CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�ʽ��˻��������������Ӧ

  virtual void OnRspTradingAccountPasswordUpdate(
      CThostFtdcTradingAccountPasswordUpdateField
          *pTradingAccountPasswordUpdate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///����¼��������Ӧ

  virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                CThostFtdcRspInfoField *pRspInfo,
                                int nRequestID, bool bIsLast){};

  ///Ԥ��¼��������Ӧ

  virtual void OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder,
                                      CThostFtdcRspInfoField *pRspInfo,
                                      int nRequestID, bool bIsLast){};

  ///Ԥ�񳷵�¼��������Ӧ

  virtual void OnRspParkedOrderAction(
      CThostFtdcParkedOrderActionField *pParkedOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///��������������Ӧ

  virtual void OnRspOrderAction(
      CThostFtdcInputOrderActionField *pInputOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///��ѯ��󱨵�������Ӧ

  virtual void OnRspQueryMaxOrderVolume(
      CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///Ͷ���߽�����ȷ����Ӧ

  virtual void OnRspSettlementInfoConfirm(
      CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///ɾ��Ԥ����Ӧ

  virtual void OnRspRemoveParkedOrder(
      CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///ɾ��Ԥ�񳷵���Ӧ

  virtual void OnRspRemoveParkedOrderAction(
      CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///ִ�����¼��������Ӧ

  virtual void OnRspExecOrderInsert(
      CThostFtdcInputExecOrderField *pInputExecOrder,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///ִ��������������Ӧ

  virtual void OnRspExecOrderAction(
      CThostFtdcInputExecOrderActionField *pInputExecOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///ѯ��¼��������Ӧ

  virtual void OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote,
                                   CThostFtdcRspInfoField *pRspInfo,
                                   int nRequestID, bool bIsLast){};

  ///����¼��������Ӧ

  virtual void OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote,
                                CThostFtdcRspInfoField *pRspInfo,
                                int nRequestID, bool bIsLast){};

  ///���۲���������Ӧ

  virtual void OnRspQuoteAction(
      CThostFtdcInputQuoteActionField *pInputQuoteAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�������¼��������Ӧ

  virtual void OnRspCombActionInsert(
      CThostFtdcInputCombActionField *pInputCombAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ������Ӧ

  virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder,
                             CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                             bool bIsLast){};

  ///�����ѯ�ɽ���Ӧ

  virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade,
                             CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                             bool bIsLast){};

  ///�����ѯͶ���ֲ߳���Ӧ

  virtual void OnRspQryInvestorPosition(
      CThostFtdcInvestorPositionField *pInvestorPosition,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ�ʽ��˻���Ӧ

  virtual void OnRspQryTradingAccount(
      CThostFtdcTradingAccountField *pTradingAccount,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯͶ������Ӧ

  virtual void OnRspQryInvestor(CThostFtdcInvestorField *pInvestor,
                                CThostFtdcRspInfoField *pRspInfo,
                                int nRequestID, bool bIsLast){};

  ///�����ѯ���ױ�����Ӧ

  virtual void OnRspQryTradingCode(CThostFtdcTradingCodeField *pTradingCode,
                                   CThostFtdcRspInfoField *pRspInfo,
                                   int nRequestID, bool bIsLast){};

  ///�����ѯ��Լ��֤������Ӧ

  virtual void OnRspQryInstrumentMarginRate(
      CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ��Լ���������Ӧ

  virtual void OnRspQryInstrumentCommissionRate(
      CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ��������Ӧ

  virtual void OnRspQryExchange(CThostFtdcExchangeField *pExchange,
                                CThostFtdcRspInfoField *pRspInfo,
                                int nRequestID, bool bIsLast){};

  ///�����ѯ��Ʒ��Ӧ

  virtual void OnRspQryProduct(CThostFtdcProductField *pProduct,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast){};

  ///�����ѯ��Լ��Ӧ

  virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
                                  CThostFtdcRspInfoField *pRspInfo,
                                  int nRequestID, bool bIsLast){};

  ///�����ѯ������Ӧ

  virtual void OnRspQryDepthMarketData(
      CThostFtdcDepthMarketDataField *pDepthMarketData,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯͶ���߽�������Ӧ

  virtual void OnRspQrySettlementInfo(
      CThostFtdcSettlementInfoField *pSettlementInfo,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯת��������Ӧ

  virtual void OnRspQryTransferBank(CThostFtdcTransferBankField *pTransferBank,
                                    CThostFtdcRspInfoField *pRspInfo,
                                    int nRequestID, bool bIsLast){};

  ///�����ѯͶ���ֲ߳���ϸ��Ӧ

  virtual void OnRspQryInvestorPositionDetail(
      CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ�ͻ�֪ͨ��Ӧ

  virtual void OnRspQryNotice(CThostFtdcNoticeField *pNotice,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast){};

  ///�����ѯ������Ϣȷ����Ӧ

  virtual void OnRspQrySettlementInfoConfirm(
      CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯͶ���ֲ߳���ϸ��Ӧ

  virtual void OnRspQryInvestorPositionCombineDetail(
      CThostFtdcInvestorPositionCombineDetailField
          *pInvestorPositionCombineDetail,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///��ѯ��֤����ϵͳ���͹�˾�ʽ��˻���Կ��Ӧ

  virtual void OnRspQryCFMMCTradingAccountKey(
      CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ�ֵ��۵���Ϣ��Ӧ

  virtual void OnRspQryEWarrantOffset(
      CThostFtdcEWarrantOffsetField *pEWarrantOffset,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯͶ����Ʒ��/��Ʒ�ֱ�֤����Ӧ

  virtual void OnRspQryInvestorProductGroupMargin(
      CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ������֤������Ӧ

  virtual void OnRspQryExchangeMarginRate(
      CThostFtdcExchangeMarginRateField *pExchangeMarginRate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ���������֤������Ӧ

  virtual void OnRspQryExchangeMarginRateAdjust(
      CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ������Ӧ

  virtual void OnRspQryExchangeRate(CThostFtdcExchangeRateField *pExchangeRate,
                                    CThostFtdcRspInfoField *pRspInfo,
                                    int nRequestID, bool bIsLast){};

  ///�����ѯ�����������Ա����Ȩ����Ӧ

  virtual void OnRspQrySecAgentACIDMap(
      CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ��Ʒ���ۻ���

  virtual void OnRspQryProductExchRate(
      CThostFtdcProductExchRateField *pProductExchRate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ��Ȩ���׳ɱ���Ӧ

  virtual void OnRspQryOptionInstrTradeCost(
      CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ��Ȩ��Լ�������Ӧ

  virtual void OnRspQryOptionInstrCommRate(
      CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯִ�������Ӧ

  virtual void OnRspQryExecOrder(CThostFtdcExecOrderField *pExecOrder,
                                 CThostFtdcRspInfoField *pRspInfo,
                                 int nRequestID, bool bIsLast){};

  ///�����ѯѯ����Ӧ

  virtual void OnRspQryForQuote(CThostFtdcForQuoteField *pForQuote,
                                CThostFtdcRspInfoField *pRspInfo,
                                int nRequestID, bool bIsLast){};

  ///�����ѯ������Ӧ

  virtual void OnRspQryQuote(CThostFtdcQuoteField *pQuote,
                             CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                             bool bIsLast){};

  ///�����ѯ��Ϻ�Լ��ȫϵ����Ӧ

  virtual void OnRspQryCombInstrumentGuard(
      CThostFtdcCombInstrumentGuardField *pCombInstrumentGuard,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ���������Ӧ

  virtual void OnRspQryCombAction(CThostFtdcCombActionField *pCombAction,
                                  CThostFtdcRspInfoField *pRspInfo,
                                  int nRequestID, bool bIsLast){};

  ///�����ѯת����ˮ��Ӧ

  virtual void OnRspQryTransferSerial(
      CThostFtdcTransferSerialField *pTransferSerial,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ����ǩԼ��ϵ��Ӧ

  virtual void OnRspQryAccountregister(
      CThostFtdcAccountregisterField *pAccountregister,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///����Ӧ��

  virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast){};

  ///����֪ͨ

  virtual void OnRtnOrder(CThostFtdcOrderField *pOrder){};

  ///�ɽ�֪ͨ

  virtual void OnRtnTrade(CThostFtdcTradeField *pTrade){};

  ///����¼�����ر�

  virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                   CThostFtdcRspInfoField *pRspInfo){};

  ///������������ر�

  virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction,
                                   CThostFtdcRspInfoField *pRspInfo){};

  ///��Լ����״̬֪ͨ

  virtual void OnRtnInstrumentStatus(
      CThostFtdcInstrumentStatusField *pInstrumentStatus){};

  ///����֪ͨ

  virtual void OnRtnTradingNotice(
      CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo){};

  ///��ʾ������У�����

  virtual void OnRtnErrorConditionalOrder(
      CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder){};

  ///ִ�����֪ͨ

  virtual void OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder){};

  ///ִ�����¼�����ر�

  virtual void OnErrRtnExecOrderInsert(
      CThostFtdcInputExecOrderField *pInputExecOrder,
      CThostFtdcRspInfoField *pRspInfo){};

  ///ִ������������ر�

  virtual void OnErrRtnExecOrderAction(
      CThostFtdcExecOrderActionField *pExecOrderAction,
      CThostFtdcRspInfoField *pRspInfo){};

  ///ѯ��¼�����ر�

  virtual void OnErrRtnForQuoteInsert(
      CThostFtdcInputForQuoteField *pInputForQuote,
      CThostFtdcRspInfoField *pRspInfo){};

  ///����֪ͨ

  virtual void OnRtnQuote(CThostFtdcQuoteField *pQuote){};

  ///����¼�����ر�

  virtual void OnErrRtnQuoteInsert(CThostFtdcInputQuoteField *pInputQuote,
                                   CThostFtdcRspInfoField *pRspInfo){};

  ///���۲�������ر�

  virtual void OnErrRtnQuoteAction(CThostFtdcQuoteActionField *pQuoteAction,
                                   CThostFtdcRspInfoField *pRspInfo){};

  ///ѯ��֪ͨ

  virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp){};

  ///��֤���������û�����

  virtual void OnRtnCFMMCTradingAccountToken(
      CThostFtdcCFMMCTradingAccountTokenField *pCFMMCTradingAccountToken){};

  ///�������֪ͨ

  virtual void OnRtnCombAction(CThostFtdcCombActionField *pCombAction){};

  ///�������¼�����ر�

  virtual void OnErrRtnCombActionInsert(
      CThostFtdcInputCombActionField *pInputCombAction,
      CThostFtdcRspInfoField *pRspInfo){};

  ///�����ѯǩԼ������Ӧ

  virtual void OnRspQryContractBank(CThostFtdcContractBankField *pContractBank,
                                    CThostFtdcRspInfoField *pRspInfo,
                                    int nRequestID, bool bIsLast){};

  ///�����ѯԤ����Ӧ

  virtual void OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder,
                                   CThostFtdcRspInfoField *pRspInfo,
                                   int nRequestID, bool bIsLast){};

  ///�����ѯԤ�񳷵���Ӧ

  virtual void OnRspQryParkedOrderAction(
      CThostFtdcParkedOrderActionField *pParkedOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ����֪ͨ��Ӧ

  virtual void OnRspQryTradingNotice(
      CThostFtdcTradingNoticeField *pTradingNotice,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ���͹�˾���ײ�����Ӧ

  virtual void OnRspQryBrokerTradingParams(
      CThostFtdcBrokerTradingParamsField *pBrokerTradingParams,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ���͹�˾�����㷨��Ӧ

  virtual void OnRspQryBrokerTradingAlgos(
      CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�����ѯ��������û�����

  virtual void OnRspQueryCFMMCTradingAccountToken(
      CThostFtdcQueryCFMMCTradingAccountTokenField
          *pQueryCFMMCTradingAccountToken,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///���з��������ʽ�ת�ڻ�֪ͨ

  virtual void OnRtnFromBankToFutureByBank(
      CThostFtdcRspTransferField *pRspTransfer){};

  ///���з����ڻ��ʽ�ת����֪ͨ

  virtual void OnRtnFromFutureToBankByBank(
      CThostFtdcRspTransferField *pRspTransfer){};

  ///���з����������ת�ڻ�֪ͨ

  virtual void OnRtnRepealFromBankToFutureByBank(
      CThostFtdcRspRepealField *pRspRepeal){};

  ///���з�������ڻ�ת����֪ͨ

  virtual void OnRtnRepealFromFutureToBankByBank(
      CThostFtdcRspRepealField *pRspRepeal){};

  ///�ڻ����������ʽ�ת�ڻ�֪ͨ

  virtual void OnRtnFromBankToFutureByFuture(
      CThostFtdcRspTransferField *pRspTransfer){};

  ///�ڻ������ڻ��ʽ�ת����֪ͨ

  virtual void OnRtnFromFutureToBankByFuture(
      CThostFtdcRspTransferField *pRspTransfer){};

  ///ϵͳ����ʱ�ڻ����ֹ������������ת�ڻ��������д�����Ϻ��̷��ص�֪ͨ

  virtual void OnRtnRepealFromBankToFutureByFutureManual(
      CThostFtdcRspRepealField *pRspRepeal){};

  ///ϵͳ����ʱ�ڻ����ֹ���������ڻ�ת�����������д�����Ϻ��̷��ص�֪ͨ

  virtual void OnRtnRepealFromFutureToBankByFutureManual(
      CThostFtdcRspRepealField *pRspRepeal){};

  ///�ڻ������ѯ�������֪ͨ

  virtual void OnRtnQueryBankBalanceByFuture(
      CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount){};

  ///�ڻ����������ʽ�ת�ڻ�����ر�

  virtual void OnErrRtnBankToFutureByFuture(
      CThostFtdcReqTransferField *pReqTransfer,
      CThostFtdcRspInfoField *pRspInfo){};

  ///�ڻ������ڻ��ʽ�ת���д���ر�

  virtual void OnErrRtnFutureToBankByFuture(
      CThostFtdcReqTransferField *pReqTransfer,
      CThostFtdcRspInfoField *pRspInfo){};

  ///ϵͳ����ʱ�ڻ����ֹ������������ת�ڻ�����ر�

  virtual void OnErrRtnRepealBankToFutureByFutureManual(
      CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo){};

  ///ϵͳ����ʱ�ڻ����ֹ���������ڻ�ת���д���ر�

  virtual void OnErrRtnRepealFutureToBankByFutureManual(
      CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo){};

  ///�ڻ������ѯ����������ر�

  virtual void OnErrRtnQueryBankBalanceByFuture(
      CThostFtdcReqQueryAccountField *pReqQueryAccount,
      CThostFtdcRspInfoField *pRspInfo){};

  ///�ڻ������������ת�ڻ��������д�����Ϻ��̷��ص�֪ͨ

  virtual void OnRtnRepealFromBankToFutureByFuture(
      CThostFtdcRspRepealField *pRspRepeal){};

  ///�ڻ���������ڻ�ת�����������д�����Ϻ��̷��ص�֪ͨ

  virtual void OnRtnRepealFromFutureToBankByFuture(
      CThostFtdcRspRepealField *pRspRepeal){};

  ///�ڻ����������ʽ�ת�ڻ�Ӧ��

  virtual void OnRspFromBankToFutureByFuture(
      CThostFtdcReqTransferField *pReqTransfer,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�ڻ������ڻ��ʽ�ת����Ӧ��

  virtual void OnRspFromFutureToBankByFuture(
      CThostFtdcReqTransferField *pReqTransfer,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///�ڻ������ѯ�������Ӧ��

  virtual void OnRspQueryBankAccountMoneyByFuture(
      CThostFtdcReqQueryAccountField *pReqQueryAccount,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

  ///���з������ڿ���֪ͨ

  virtual void OnRtnOpenAccountByBank(
      CThostFtdcOpenAccountField *pOpenAccount){};

  ///���з���������֪ͨ

  virtual void OnRtnCancelAccountByBank(
      CThostFtdcCancelAccountField *pCancelAccount){};

  ///���з����������˺�֪ͨ

  virtual void OnRtnChangeAccountByBank(
      CThostFtdcChangeAccountField *pChangeAccount){};
};

class TRADER_API_EXPORT CThostFtdcTraderApi {
 public:
  ///����TraderApi
  ///@param pszFlowPath ��������Ϣ�ļ���Ŀ¼��Ĭ��Ϊ��ǰĿ¼
  ///@return ��������UserApi
  static CThostFtdcTraderApi *CreateFtdcTraderApi(const char *pszFlowPath = "");

  ///��ȡAPI�İ汾��Ϣ
  ///@retrun ��ȡ���İ汾��
  static const char *GetApiVersion();

  ///ɾ��ӿڶ�����
  ///@remark ����ʹ�ñ��ӿڶ���ʱ,���øú���ɾ��ӿڶ���
  virtual void Release() = 0;

  ///��ʼ��
  ///@remark ��ʼ�����л���,ֻ�е��ú�,�ӿڲſ�ʼ����
  virtual void Init() = 0;

  ///�ȴ�ӿ��߳̽�������
  ///@return �߳��˳�����
  virtual int Join() = 0;

  ///��ȡ��ǰ������
  ///@retrun ��ȡ���Ľ�����
  ///@remark ֻ�е�¼�ɹ���,���ܵõ���ȷ�Ľ�����
  virtual const char *GetTradingDay() = 0;

  ///ע��ǰ�û������ַ
  ///@param pszFrontAddress��ǰ�û������ַ��
  ///@remark
  ///�����ַ�ĸ�ʽΪ����protocol://ipaddress:port�����磺��tcp://127.0.0.1:17001����
  ///@remark ��tcp����?��Э�飬��127.0.0.1������������ַ����17001�����������˿ںš�
  virtual void RegisterFront(char *pszFrontAddress) = 0;

  ///ע�����ַ����������ַ
  ///@param pszNsAddress�����ַ����������ַ��
  ///@remark
  ///�����ַ�ĸ�ʽΪ����protocol://ipaddress:port�����磺��tcp://127.0.0.1:12001����
  ///@remark ��tcp����?��Э�飬��127.0.0.1������������ַ����12001�����������˿ںš�
  ///@remark RegisterNameServer������RegisterFront
  virtual void RegisterNameServer(char *pszNsAddress) = 0;

  ///ע�����ַ������û���Ϣ
  ///@param pFensUserInfo���û���Ϣ��
  virtual void RegisterFensUserInfo(
      CThostFtdcFensUserInfoField *pFensUserInfo) = 0;

  ///ע��ص��ӿ�
  ///@param pSpi �����Իص��ӿ����ʵ��
  virtual void RegisterSpi(CThostFtdcTraderSpi *pSpi) = 0;

  ///����˽������
  ///@param nResumeType ˽�����ش���ʽ
  ///        THOST_TERT_RESTART:�ӱ������տ�ʼ�ش�
  ///        THOST_TERT_RESUME:���ϴ��յ�����
  ///        THOST_TERT_QUICK:ֻ���͵�¼��˽����������
  ///@remark
  ///�÷���Ҫ��Init����ǰ���á����������򲻻��յ�˽��������ݡ�
  virtual void SubscribePrivateTopic(THOST_TE_RESUME_TYPE nResumeType) = 0;

  ///���Ĺ�������
  ///@param nResumeType �������ش���ʽ
  ///        THOST_TERT_RESTART:�ӱ������տ�ʼ�ش�
  ///        THOST_TERT_RESUME:���ϴ��յ�����
  ///        THOST_TERT_QUICK:ֻ���͵�¼�󹫹���������
  ///@remark
  ///�÷���Ҫ��Init����ǰ���á����������򲻻��յ�����������ݡ�
  virtual void SubscribePublicTopic(THOST_TE_RESUME_TYPE nResumeType) = 0;

  ///�ͻ�����֤����
  virtual int ReqAuthenticate(
      CThostFtdcReqAuthenticateField *pReqAuthenticateField,
      int nRequestID) = 0;

  ///�û���¼����
  virtual int ReqUserLogin(CThostFtdcReqUserLoginField *pReqUserLoginField,
                           int nRequestID) = 0;

  ///�ǳ�����
  virtual int ReqUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                            int nRequestID) = 0;

  ///�û������������
  virtual int ReqUserPasswordUpdate(
      CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate,
      int nRequestID) = 0;

  ///�ʽ��˻������������
  virtual int ReqTradingAccountPasswordUpdate(
      CThostFtdcTradingAccountPasswordUpdateField
          *pTradingAccountPasswordUpdate,
      int nRequestID) = 0;

  ///����¼������
  virtual int ReqOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                             int nRequestID) = 0;

  ///Ԥ��¼������
  virtual int ReqParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder,
                                   int nRequestID) = 0;

  ///Ԥ�񳷵�¼������
  virtual int ReqParkedOrderAction(
      CThostFtdcParkedOrderActionField *pParkedOrderAction, int nRequestID) = 0;

  ///������������
  virtual int ReqOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction,
                             int nRequestID) = 0;

  ///��ѯ��󱨵���������
  virtual int ReqQueryMaxOrderVolume(
      CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume,
      int nRequestID) = 0;

  ///Ͷ���߽�����ȷ��
  virtual int ReqSettlementInfoConfirm(
      CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
      int nRequestID) = 0;

  ///����ɾ��Ԥ��
  virtual int ReqRemoveParkedOrder(
      CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID) = 0;

  ///����ɾ��Ԥ�񳷵�
  virtual int ReqRemoveParkedOrderAction(
      CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction,
      int nRequestID) = 0;

  ///ִ�����¼������
  virtual int ReqExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder,
                                 int nRequestID) = 0;

  ///ִ������������
  virtual int ReqExecOrderAction(
      CThostFtdcInputExecOrderActionField *pInputExecOrderAction,
      int nRequestID) = 0;

  ///ѯ��¼������
  virtual int ReqForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote,
                                int nRequestID) = 0;

  ///����¼������
  virtual int ReqQuoteInsert(CThostFtdcInputQuoteField *pInputQuote,
                             int nRequestID) = 0;

  ///���۲�������
  virtual int ReqQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction,
                             int nRequestID) = 0;

  ///�������¼������
  virtual int ReqCombActionInsert(
      CThostFtdcInputCombActionField *pInputCombAction, int nRequestID) = 0;

  ///�����ѯ����
  virtual int ReqQryOrder(CThostFtdcQryOrderField *pQryOrder,
                          int nRequestID) = 0;

  ///�����ѯ�ɽ�
  virtual int ReqQryTrade(CThostFtdcQryTradeField *pQryTrade,
                          int nRequestID) = 0;

  ///�����ѯͶ���ֲ߳�
  virtual int ReqQryInvestorPosition(
      CThostFtdcQryInvestorPositionField *pQryInvestorPosition,
      int nRequestID) = 0;

  ///�����ѯ�ʽ��˻�
  virtual int ReqQryTradingAccount(
      CThostFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID) = 0;

  ///�����ѯͶ����
  virtual int ReqQryInvestor(CThostFtdcQryInvestorField *pQryInvestor,
                             int nRequestID) = 0;

  ///�����ѯ���ױ���
  virtual int ReqQryTradingCode(CThostFtdcQryTradingCodeField *pQryTradingCode,
                                int nRequestID) = 0;

  ///�����ѯ��Լ��֤����
  virtual int ReqQryInstrumentMarginRate(
      CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate,
      int nRequestID) = 0;

  ///�����ѯ��Լ�������
  virtual int ReqQryInstrumentCommissionRate(
      CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate,
      int nRequestID) = 0;

  ///�����ѯ������
  virtual int ReqQryExchange(CThostFtdcQryExchangeField *pQryExchange,
                             int nRequestID) = 0;

  ///�����ѯ��Ʒ
  virtual int ReqQryProduct(CThostFtdcQryProductField *pQryProduct,
                            int nRequestID) = 0;

  ///�����ѯ��Լ
  virtual int ReqQryInstrument(CThostFtdcQryInstrumentField *pQryInstrument,
                               int nRequestID) = 0;

  ///�����ѯ����
  virtual int ReqQryDepthMarketData(
      CThostFtdcQryDepthMarketDataField *pQryDepthMarketData,
      int nRequestID) = 0;

  ///�����ѯͶ���߽�����
  virtual int ReqQrySettlementInfo(
      CThostFtdcQrySettlementInfoField *pQrySettlementInfo, int nRequestID) = 0;

  ///�����ѯת������
  virtual int ReqQryTransferBank(
      CThostFtdcQryTransferBankField *pQryTransferBank, int nRequestID) = 0;

  ///�����ѯͶ���ֲ߳���ϸ
  virtual int ReqQryInvestorPositionDetail(
      CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail,
      int nRequestID) = 0;

  ///�����ѯ�ͻ�֪ͨ
  virtual int ReqQryNotice(CThostFtdcQryNoticeField *pQryNotice,
                           int nRequestID) = 0;

  ///�����ѯ������Ϣȷ��
  virtual int ReqQrySettlementInfoConfirm(
      CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm,
      int nRequestID) = 0;

  ///�����ѯͶ���ֲ߳���ϸ
  virtual int ReqQryInvestorPositionCombineDetail(
      CThostFtdcQryInvestorPositionCombineDetailField
          *pQryInvestorPositionCombineDetail,
      int nRequestID) = 0;

  ///�����ѯ��֤����ϵͳ���͹�˾�ʽ��˻���Կ
  virtual int ReqQryCFMMCTradingAccountKey(
      CThostFtdcQryCFMMCTradingAccountKeyField *pQryCFMMCTradingAccountKey,
      int nRequestID) = 0;

  ///�����ѯ�ֵ��۵���Ϣ
  virtual int ReqQryEWarrantOffset(
      CThostFtdcQryEWarrantOffsetField *pQryEWarrantOffset, int nRequestID) = 0;

  ///�����ѯͶ����Ʒ��/��Ʒ�ֱ�֤��
  virtual int ReqQryInvestorProductGroupMargin(
      CThostFtdcQryInvestorProductGroupMarginField
          *pQryInvestorProductGroupMargin,
      int nRequestID) = 0;

  ///�����ѯ������֤����
  virtual int ReqQryExchangeMarginRate(
      CThostFtdcQryExchangeMarginRateField *pQryExchangeMarginRate,
      int nRequestID) = 0;

  ///�����ѯ���������֤����
  virtual int ReqQryExchangeMarginRateAdjust(
      CThostFtdcQryExchangeMarginRateAdjustField *pQryExchangeMarginRateAdjust,
      int nRequestID) = 0;

  ///�����ѯ����
  virtual int ReqQryExchangeRate(
      CThostFtdcQryExchangeRateField *pQryExchangeRate, int nRequestID) = 0;

  ///�����ѯ�����������Ա����Ȩ��
  virtual int ReqQrySecAgentACIDMap(
      CThostFtdcQrySecAgentACIDMapField *pQrySecAgentACIDMap,
      int nRequestID) = 0;

  ///�����ѯ��Ʒ���ۻ���
  virtual int ReqQryProductExchRate(
      CThostFtdcQryProductExchRateField *pQryProductExchRate,
      int nRequestID) = 0;

  ///�����ѯ��Ȩ���׳ɱ�
  virtual int ReqQryOptionInstrTradeCost(
      CThostFtdcQryOptionInstrTradeCostField *pQryOptionInstrTradeCost,
      int nRequestID) = 0;

  ///�����ѯ��Ȩ��Լ�����
  virtual int ReqQryOptionInstrCommRate(
      CThostFtdcQryOptionInstrCommRateField *pQryOptionInstrCommRate,
      int nRequestID) = 0;

  ///�����ѯִ�����
  virtual int ReqQryExecOrder(CThostFtdcQryExecOrderField *pQryExecOrder,
                              int nRequestID) = 0;

  ///�����ѯѯ��
  virtual int ReqQryForQuote(CThostFtdcQryForQuoteField *pQryForQuote,
                             int nRequestID) = 0;

  ///�����ѯ����
  virtual int ReqQryQuote(CThostFtdcQryQuoteField *pQryQuote,
                          int nRequestID) = 0;

  ///�����ѯ��Ϻ�Լ��ȫϵ��
  virtual int ReqQryCombInstrumentGuard(
      CThostFtdcQryCombInstrumentGuardField *pQryCombInstrumentGuard,
      int nRequestID) = 0;

  ///�����ѯ�������
  virtual int ReqQryCombAction(CThostFtdcQryCombActionField *pQryCombAction,
                               int nRequestID) = 0;

  ///�����ѯת����ˮ
  virtual int ReqQryTransferSerial(
      CThostFtdcQryTransferSerialField *pQryTransferSerial, int nRequestID) = 0;

  ///�����ѯ����ǩԼ��ϵ
  virtual int ReqQryAccountregister(
      CThostFtdcQryAccountregisterField *pQryAccountregister,
      int nRequestID) = 0;

  ///�����ѯǩԼ����
  virtual int ReqQryContractBank(
      CThostFtdcQryContractBankField *pQryContractBank, int nRequestID) = 0;

  ///�����ѯԤ��
  virtual int ReqQryParkedOrder(CThostFtdcQryParkedOrderField *pQryParkedOrder,
                                int nRequestID) = 0;

  ///�����ѯԤ�񳷵�
  virtual int ReqQryParkedOrderAction(
      CThostFtdcQryParkedOrderActionField *pQryParkedOrderAction,
      int nRequestID) = 0;

  ///�����ѯ����֪ͨ
  virtual int ReqQryTradingNotice(
      CThostFtdcQryTradingNoticeField *pQryTradingNotice, int nRequestID) = 0;

  ///�����ѯ���͹�˾���ײ���
  virtual int ReqQryBrokerTradingParams(
      CThostFtdcQryBrokerTradingParamsField *pQryBrokerTradingParams,
      int nRequestID) = 0;

  ///�����ѯ���͹�˾�����㷨
  virtual int ReqQryBrokerTradingAlgos(
      CThostFtdcQryBrokerTradingAlgosField *pQryBrokerTradingAlgos,
      int nRequestID) = 0;

  ///�����ѯ��������û�����
  virtual int ReqQueryCFMMCTradingAccountToken(
      CThostFtdcQueryCFMMCTradingAccountTokenField
          *pQueryCFMMCTradingAccountToken,
      int nRequestID) = 0;

  ///�ڻ����������ʽ�ת�ڻ�����
  virtual int ReqFromBankToFutureByFuture(
      CThostFtdcReqTransferField *pReqTransfer, int nRequestID) = 0;

  ///�ڻ������ڻ��ʽ�ת��������
  virtual int ReqFromFutureToBankByFuture(
      CThostFtdcReqTransferField *pReqTransfer, int nRequestID) = 0;

  ///�ڻ������ѯ�����������
  virtual int ReqQueryBankAccountMoneyByFuture(
      CThostFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID) = 0;

 protected:
  ~CThostFtdcTraderApi(){};
};

#endif
