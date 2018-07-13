// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef INTERFACES_SIGNAL_SIGNAL_POOL_H_
#define INTERFACES_SIGNAL_SIGNAL_POOL_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "interfaces/data/data_pool.h"
#include "interfaces/signal/proto/signal_config.pb.h"
#include "interfaces/signal/signal_base.h"
#include "utils/graph.h"

namespace interface {

typedef std::unordered_map<std::string, SignalConfig> NameToSignalConfig;
typedef std::unordered_map<std::string, std::unique_ptr<SignalBase>>
    NameToSignal;
typedef std::unordered_map<std::string, util::Graph> TimeToGraph;

class SignalPool {
 public:
  SignalPool(const SignalConfigs& signal_configs, DataPool* data_pool);
  virtual ~SignalPool();

  SignalConfigs GetSignalConfigs();
  SignalConfig GetSignalConfigByName(const std::string& signal_name);
  SignalBase* GetSignalByName(const std::string& signal_name);

  util::SerializedGraph GetSerializedGraph();
  TimeToGraph GetBeginGraph();
  TimeToGraph GetEndGraph();

 private:
  NameToSignal name_to_signal_;
  NameToSignalConfig name_to_signal_config_;
  SignalConfigs signal_configs_;
  TimeToGraph begin_to_graph_;
  TimeToGraph end_to_graph_;
  util::Graph graph_;
};

}  // namespace interface

#endif  // INTERFACES_SIGNAL_SIGNAL_POOL_H_
