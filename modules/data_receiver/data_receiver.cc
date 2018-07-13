// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "modules/data_receiver/data_receiver.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <glog/logging.h>

#include "utils/date_time.h"

using boost::gregorian::date;
using boost::gregorian::day_clock;
using boost::posix_time::duration_from_string;
using boost::posix_time::ptime;
using boost::posix_time::second_clock;
using boost::posix_time::seconds;
using boost::posix_time::time_duration;
using boost::posix_time::to_simple_string;
using event_scheduler::Action;
using event_scheduler::Event;
using interface::Data;
using interface::DataPool;
using interface::Security;
using std::move;
using std::string;
using std::unique_ptr;
using util::Queue;
using util::ToString;

namespace data_receiver {
namespace {

const int32_t kSecondsPerDay = 24 * 3600;
const int32_t kTimeToWaitInSeconds = 5 * 60;
const int32_t kLateQuotaInSeconds = 60 * 60;

}  // namespace

DataReceiver::DataReceiver(const DataReceiverConfig& config,
                           DataPool* data_pool, Queue<Event*>* event_queue,
                           Queue<DataMessage>* data_message_queue)
    : config_(config), data_pool_(data_pool), event_queue_(event_queue) {
  // Create CtpFutureDataClients.
  for (const auto& client_config : config.ctp_future_data_client_config()) {
    name_to_data_client_[client_config.name()].reset(new CtpFutureDataClient(
        client_config, data_pool, event_queue, data_message_queue));
  }
}

DataReceiver::~DataReceiver() {}

void DataReceiver::Init() {
  LOG(INFO) << "Initing data receiver";

  // Schedule "dump data" events.
  date today = day_clock::local_day();

  for (const auto& id_to_data : *data_pool_) {
    Data* data = id_to_data.second.get();

    if (data == nullptr) {
      LOG(ERROR) << "Data is nullptr for <" << id_to_data.first << ">";

      continue;
    }

    const Security& security = data->GetSecurity();

    for (const auto& time_interval : security.time_interval()) {
      time_duration end_time = duration_from_string(time_interval.end_time()) +
                               seconds(kTimeToWaitInSeconds);

      Action action = [this, security, data, end_time] {
        string end_time_string = to_simple_string(end_time);
        time_duration now = second_clock::local_time().time_of_day();

        // It takes some time to dump data for on security.
        // If kLateQuotaInSeconds is too short,
        // data of some securities could not be dumped.
        if (now > end_time + seconds(kLateQuotaInSeconds)) {
        	LOG(ERROR) << "Deadline passed for dumping <" << security.id() << "> at <" << end_time_string << ">";
        	
          return;
        }

        LOG(INFO) << "Start dumping data for <" << security.id() << "> at <"
                  << end_time_string << ">";

        data->Dump(this->config_.data_dir(), end_time_string,
                   this->config_.database_config());

        LOG(INFO) << "Finish dumping data for <" << security.id() << "> at <"
                  << end_time_string << ">";
      };

	    ptime start_time(today, end_time);
	    
      unique_ptr<Event> event(new Event(action, start_time, kSecondsPerDay,
                                        "Dump data for <" + security.id() +
                                            "> at <" +
                                            to_simple_string(end_time) + ">"));
      events_.emplace_back(move(event));

      event_queue_->Insert(events_.back().get());
      event_queue_->Notify();
    }
  }

  // All clients should implement Init() method.
  for (const auto& name_to_data_client : name_to_data_client_) {
    name_to_data_client.second->Init();
  }

  LOG(INFO) << "Inited data receiver";
}

void DataReceiver::Start() {
  LOG(INFO) << "Starting data receiver";

  // All clients should implement Start() method.
  for (const auto& name_to_data_client : name_to_data_client_) {
    client_threads_.create_thread([&name_to_data_client] {
      if (name_to_data_client.second != nullptr) {
        name_to_data_client.second->Start();
      }
    });
  }

  LOG(INFO) << "Started data receiver";
}

void DataReceiver::Exit() {
  LOG(INFO) << "Exiting data receiver";

  // All clients should implement Exit() method.
  for (const auto& name_to_data_client : name_to_data_client_) {
    name_to_data_client.second->Exit();
  }

  client_threads_.join_all();

  // Cancel all dump events.
  for (const auto& event : events_) {
    event->Cancel();
  }

  LOG(INFO) << "Exited data receiver";
}

}  // namespace data_receiver