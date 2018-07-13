// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "interfaces/signal/signal_pool.h"

#include <algorithm>

#include <glog/logging.h>

#include "interfaces/signal/signal_moving_average.h"
#include "interfaces/signal/signal_simple.h"

using std::string;
using util::Graph;
using util::SerializedGraph;

namespace interface {

SignalPool::SignalPool(const SignalConfigs& signal_configs, DataPool* data_pool)
    : signal_configs_(signal_configs) {
  for (const auto& config : signal_configs.signal_config()) {
    name_to_signal_config_[config.name()] = config;

    graph_.InsertNode(config.name());

    switch (config.signal_type()) {
      case SIMPLE:
        break;
      case MOVING_AVERAGE:
        graph_.InsertEdge(config.name(), config.data_signal_name());
        graph_.InsertEdge(config.name(), config.time_signal_name());
        break;
      default:
        break;
    }
  }

  for (const auto& config : signal_configs.signal_config()) {
    // If signal A depends on signal B,
    // then we have an edge from B to A.
    for (const auto& time_interval : config.time_interval()) {
      Graph& begin_graph = begin_to_graph_[time_interval.begin_time()];
      begin_graph.InsertNode(config.name());

      Graph& end_graph = end_to_graph_[time_interval.end_time()];
      end_graph.InsertNode(config.name());

      switch (config.signal_type()) {
        case SIMPLE:
          break;
        case MOVING_AVERAGE:
          graph_.InsertEdge(config.name(), config.data_signal_name());
          graph_.InsertEdge(config.name(), config.time_signal_name());

          begin_graph.InsertEdge(config.name(), config.data_signal_name());
          begin_graph.InsertEdge(config.name(), config.time_signal_name());

          end_graph.InsertEdge(config.name(), config.data_signal_name());
          end_graph.InsertEdge(config.name(), config.time_signal_name());

          break;
        default:
          break;
      }
    }
  }

  for (const auto& node : graph_.SerializeGraph()) {
    if (name_to_signal_config_.count(node) == 0) {
      LOG(ERROR) << "Could not find config for signal <" << node << ">";

      continue;
    }

    const SignalConfig& signal_config = name_to_signal_config_[node];

    if (name_to_signal_.count(signal_config.name()) != 0) {
      LOG(ERROR) << "Signal <" << signal_config.name() << "> already exists";

      continue;
    }

    LOG(INFO) << "Creating signal with name <" << signal_config.name()
              << "> config <" << signal_config.DebugString() << ">";

    switch (signal_config.signal_type()) {
      case SIMPLE:
        if (data_pool->GetDataById(signal_config.data_id()) != nullptr) {
          name_to_signal_[signal_config.name()].reset(new SignalSimple(
              signal_config.name(), signal_config.time_interval(),
              data_pool->GetDataById(signal_config.data_id()),
              signal_config.data_type()));
        }
        break;
      case MOVING_AVERAGE:
        if (name_to_signal_[signal_config.time_signal_name()] != nullptr &&
            name_to_signal_[signal_config.data_signal_name()] != nullptr) {
          name_to_signal_[signal_config.name()].reset(new SignalMovingAverage(
              signal_config.name(), signal_config.num_periods(),
              signal_config.period_in_seconds(), signal_config.time_interval(),
              name_to_signal_[signal_config.time_signal_name()].get(),
              name_to_signal_[signal_config.data_signal_name()].get()));
        }
        break;
      default:
        break;
    }
  }
}

SignalPool::~SignalPool() {}

SignalConfigs SignalPool::GetSignalConfigs() { return signal_configs_; }

SignalConfig SignalPool::GetSignalConfigByName(const string& signal_name) {
  return name_to_signal_config_[signal_name];
}

SignalBase* SignalPool::GetSignalByName(const string& signal_name) {
  return name_to_signal_[signal_name].get();
}

SerializedGraph SignalPool::GetSerializedGraph() {
  return graph_.SerializeGraph();
}

TimeToGraph SignalPool::GetBeginGraph() { return begin_to_graph_; }

TimeToGraph SignalPool::GetEndGraph() { return end_to_graph_; }

}  // namespace interface
