// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "http_server/http_server_handler.h"

#include <string>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/network/uri.hpp>

#include <glog/logging.h>

#include <jsoncpp/json/json.h>

#include "modules/event_scheduler/event.h"
#include "utils/date_time.h"
#include "utils/file.h"

using Json::Int64;
using Json::Value;
using boost::algorithm::to_lower_copy;
using boost::filesystem::directory_iterator;
using boost::filesystem::exists;
using boost::filesystem::is_directory;
using boost::filesystem::is_regular_file;
using boost::filesystem::path;
using boost::is_any_of;
using boost::join;
using boost::network::http::server;
using boost::network::http::source;
using boost::network::uri::uri;
using boost::posix_time::hours;
using boost::split;
using event_scheduler::DescriptionToEvent;
using event_scheduler::Event;
using interface::Data;
using interface::DataPool;
using interface::MOVING_AVERAGE;
using interface::Security;
using interface::Signal;
using interface::SignalBase;
using interface::SignalConfigs;
using interface::SignalPool;
using interface::Snapshot;
using std::ifstream;
using std::stoi;
using std::string;
using std::to_string;
using std::unordered_map;
using std::vector;
using util::ReadFileLines;
using util::ToString;

namespace http_server {

const string kLogPath = "/log";
const string kLogType = "type";
const string kLogPage = "page";
const string kInfo = "info";
const string kWarning = "warning";
const string kError = "error";

const string kSecurityPath = "/security";
const string kSecurityId = "id";
const string kDataHeader = "header";
const string kDataTimestamp = "timestamp";
const string kDataValue = "value";
const string kDataPayload = "payload";
const string kLastPrice = "last_price";
const string kCurrentVolume = "current_volume";
const string kTotalVolume = "total_volume";
const string kLevel1BidPrice = "level_1_bid_price";
const string kLevel1BidVolume = "level_1_bid_volume";
const string kLevel1AskPrice = "level_1_ask_price";
const string kLevel1AskVolume = "level_1_ask_volume";

const string kSignalPath = "/signal";

const string kEventPath = "/event";

const string kShutDownPath = "/quitquitquit";

HttpServerHandler::HttpServerHandler(
    const HttpServerConfig& config,
    const DescriptionToEvent& description_to_event, DataPool* data_pool,
    SignalPool* signal_pool)
    : config_(config),
      description_to_event_(description_to_event),
      data_pool_(data_pool),
      signal_pool_(signal_pool) {
  path_to_handler_[kShutDownPath] = [this](const KeyToValue& key_to_value,
                                           Response& response) {
    this->HandleShutDownRequest(key_to_value, response);
  };

  path_to_handler_[kLogPath] = [this](const KeyToValue& key_to_value,
                                      Response& response) {
    this->HandleLogRequest(key_to_value, response);
  };

  path_to_handler_[kSecurityPath] = [this](const KeyToValue& key_to_value,
                                           Response& response) {
    this->HandleSecurityRequest(key_to_value, response);
  };

  path_to_handler_[kSignalPath] = [this](const KeyToValue& key_to_value,
                                         Response& response) {
    this->HandleSignalRequest(key_to_value, response);
  };

  path_to_handler_[kEventPath] = [this](const KeyToValue& key_to_value,
                                        Response& response) {
    this->HandleEventRequest(key_to_value, response);
  };
}

HttpServerHandler::~HttpServerHandler() {}

void HttpServerHandler::operator()(const Request& request, Response& response) {
  string relative_url = destination(request);
  LOG(INFO) << "URL: " << relative_url;

  string absolute_url = "http://localhost" + relative_url;
  uri url(absolute_url);
  string path = url.path();

  if (Accept(path)) {
    KeyToValue key_to_value = ParseParameters(url.query());

    path_to_handler_[path](key_to_value, response);
  } else {
    auto status = server<HttpServerHandler>::response::ok;
    string body = "Request not accepted for URL: " + relative_url;

    response = Response::stock_reply(status, body);
  }
}

void HttpServerHandler::log(
    const server<HttpServerHandler>::string_type& info) {
  LOG(INFO) << info;
}

bool HttpServerHandler::Running() { return running_; }

bool HttpServerHandler::Accept(const string& path) {
  if (path_to_handler_.find(path) != path_to_handler_.end()) {
    return true;
  } else {
    return false;
  }
}

KeyToValue HttpServerHandler::ParseParameters(const string& query) {
  vector<string> parameters;
  split(parameters, query, is_any_of("&"));

  KeyToValue key_to_value;
  for (const auto& parameter : parameters) {
    vector<string> key_and_value;
    split(key_and_value, parameter, is_any_of("="));

    if (key_and_value.size() == 2) {
      key_to_value[key_and_value[0]] = key_and_value[1];
    }
  }

  return key_to_value;
}

vector<string> HttpServerHandler::FindLogFileNames(const string& file_type) {
  vector<string> file_names;

  if (file_type != kInfo && file_type != kWarning && file_type != kError) {
    return file_names;
  }

  path file_path(config_.log_dir());

  directory_iterator end_iter;

  if (exists(file_path) && is_directory(file_path)) {
    for (directory_iterator dir_iter(file_path); dir_iter != end_iter;
         dir_iter++) {
      if (is_regular_file(dir_iter->status()) &&
          to_lower_copy(dir_iter->path().string()).find("log." + file_type) !=
              string::npos) {
        file_names.push_back(dir_iter->path().string());
      }
    }
  }

  sort(file_names.begin(), file_names.end());

  return file_names;
}

void HttpServerHandler::HandleShutDownRequest(const KeyToValue& key_to_value,
                                              Response& response) {
  running_ = false;

  response = Response::stock_reply(Response::ok, "System is shutting down");
}

void HttpServerHandler::HandleLogRequest(const KeyToValue& key_to_value,
                                         Response& response) {
  Value value;

  if (key_to_value.size() == 0) {
    vector<string> file_names = FindLogFileNames(kInfo);

    if (!file_names.empty()) {
      for (const auto& line : ReadFileLines(file_names[0])) {
        value["payload"].append(line);
      }
    }
  } else if (key_to_value.count(kLogType) != 0 &&
             key_to_value.count(kLogPage) == 0) {
    vector<string> file_names = FindLogFileNames(key_to_value.at(kLogType));

    if (!file_names.empty()) {
      for (const auto& line : ReadFileLines(file_names[0])) {
        value["payload"].append(line);
      }
    }
  } else {
    vector<string> file_names = FindLogFileNames(key_to_value.at(kLogType));

    if (!file_names.empty()) {
      try {
        uint32_t page = static_cast<uint32_t>(stoi(key_to_value.at(kLogPage)));

        page = (page < file_names.size() ? page : file_names.size()) - 1;

        for (const auto& line : ReadFileLines(file_names[page])) {
          value["payload"].append(line);
        }
      } catch (...) {
        value["payload"].append("Page number parsing error");
      }
    }
  }

  response = Response::stock_reply(Response::ok, value.toStyledString());
}

void HttpServerHandler::HandleSecurityRequest(const KeyToValue& key_to_value,
                                              Response& response) {
  string body;

  if (key_to_value.size() == 0) {
    body = GetSecurityList();
  } else if (key_to_value.count(kSecurityId) != 0) {
    body = GetData(key_to_value.at(kSecurityId));
  }

  response = Response::stock_reply(Response::ok, body);
}

string HttpServerHandler::GetSecurityList() {
  Value list;

  for (const auto& id_to_data : *data_pool_) {
    Value security_value;

    Security security = id_to_data.second->GetSecurity();

    security_value["id"] = security.id();
    security_value["name"] = security.name();
    security_value["multiplier"] = security.multiplier();
    security_value["tick"] = security.tick();
    security_value["detail"] = "/security?id=" + security.id();

    for (const auto& time_interval : security.time_interval()) {
      Value time_interval_value;

      time_interval_value["begin_time"] = time_interval.begin_time();
      time_interval_value["end_time"] = time_interval.end_time();

      security_value["time_interval"].append(time_interval_value);
    }

    list["securities"].append(security_value);
  }

  return list.toStyledString();
}

string HttpServerHandler::GetData(const string& id) {
  Data* data = data_pool_->GetDataById(id);

  if (data == nullptr) {
    return "Data for " + id + " not found";
  }

  Value value;

  value[kLastPrice][kDataHeader].append(kDataTimestamp);
  value[kLastPrice][kDataHeader].append(kDataValue);
  value[kCurrentVolume][kDataHeader].append(kDataTimestamp);
  value[kCurrentVolume][kDataHeader].append(kDataValue);
  value[kTotalVolume][kDataHeader].append(kDataTimestamp);
  value[kTotalVolume][kDataHeader].append(kDataValue);
  value[kLevel1BidPrice][kDataHeader].append(kDataTimestamp);
  value[kLevel1BidPrice][kDataHeader].append(kDataValue);
  value[kLevel1BidVolume][kDataHeader].append(kDataTimestamp);
  value[kLevel1BidVolume][kDataHeader].append(kDataValue);
  value[kLevel1AskPrice][kDataHeader].append(kDataTimestamp);
  value[kLevel1AskPrice][kDataHeader].append(kDataValue);
  value[kLevel1AskVolume][kDataHeader].append(kDataTimestamp);
  value[kLevel1AskVolume][kDataHeader].append(kDataValue);

  for (const auto& snapshot : data->GetSnapshots()) {
    Value last_price_pair;
    last_price_pair.append(snapshot.exchange_time());
    last_price_pair.append(snapshot.last_price());

    value[kLastPrice][kDataPayload].append(last_price_pair);

    Value current_volume_pair;
    current_volume_pair.append(snapshot.exchange_time());
    current_volume_pair.append(Int64(snapshot.current_volume()));

    value[kCurrentVolume][kDataPayload].append(current_volume_pair);

    Value total_volume_pair;
    total_volume_pair.append(snapshot.exchange_time());
    total_volume_pair.append(Int64(snapshot.total_volume()));

    value[kTotalVolume][kDataPayload].append(total_volume_pair);

    Value level_1_bid_price_pair;
    level_1_bid_price_pair.append(snapshot.exchange_time());
    level_1_bid_price_pair.append(snapshot.level_1_bid_price());

    value[kLevel1BidPrice][kDataPayload].append(level_1_bid_price_pair);

    Value level_1_bid_volume_pair;
    level_1_bid_volume_pair.append(snapshot.exchange_time());
    level_1_bid_volume_pair.append(Int64(snapshot.level_1_bid_volume()));

    value[kLevel1BidVolume][kDataPayload].append(level_1_bid_volume_pair);

    Value level_1_ask_price_pair;
    level_1_ask_price_pair.append(snapshot.exchange_time());
    level_1_ask_price_pair.append(snapshot.level_1_ask_price());

    value[kLevel1AskPrice][kDataPayload].append(level_1_ask_price_pair);

    Value level_1_ask_volume_pair;
    level_1_ask_volume_pair.append(snapshot.exchange_time());
    level_1_ask_volume_pair.append(Int64(snapshot.level_1_ask_volume()));

    value[kLevel1AskVolume][kDataPayload].append(level_1_ask_volume_pair);
  }

  return value.toStyledString();
}

void HttpServerHandler::HandleSignalRequest(const KeyToValue& key_to_value,
                                            Response& response) {
  response = Response::stock_reply(Response::ok, "To be implemented");
}

void HttpServerHandler::HandleEventRequest(const KeyToValue& key_to_value,
                                           Response& response) {
  Value values;

  for (const auto& description_to_event : description_to_event_) {
    Value value;

    Event* event = description_to_event.second;

    value["description"] = event->GetDescription();
    value["start_time"] = ToString(event->GetStartTime());
    value["expires_at"] = ToString(event->GetExpiresAt() + hours(8));
    value["expires_from_now"] = to_simple_string(event->GetExpiresFromNow());
    value["period_in_seconds"] = Int64(event->GetPeriodInSeconds());
    value["canceled"] = event->GetCanceled();

    values.append(value);
  }

  response = Response::stock_reply(Response::ok, values.toStyledString());
}

}  // namespace http_server
