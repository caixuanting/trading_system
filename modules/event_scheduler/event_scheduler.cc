// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include <glog/logging.h>

#include "modules/event_scheduler/event_scheduler.h"

using boost::asio::deadline_timer;
using boost::asio::io_service;
using boost::asio::placeholders::error;
using boost::posix_time::time_duration;
using boost::posix_time::second_clock;
using std::bind;
using std::thread;
using std::unique_ptr;
using util::Queue;

namespace event_scheduler {

EventScheduler::EventScheduler(const EventSchedulerConfig& config,
                               Queue<Event*>* event_queue)
    : config_(config), event_queue_(event_queue) {}

EventScheduler::~EventScheduler() {}

void EventScheduler::Init() {
  LOG(INFO) << "Initing event scheduler";

  // Run method of io_service would exit immediately if we don't have any job
  // for it.
  // Create work would stop io_service thread from exiting immediately.
  work_.reset(new io_service::work(io_service_));

  for (uint32_t i = 0; i < config_.num_threads(); i++) {
    work_threads_.create_thread([this] { this->io_service_.run(); });
  }

  running_ = true;

  LOG(INFO) << "Inited event scheduler";
}

void EventScheduler::Start() {
  LOG(INFO) << "Starting event scheduler";

  main_thread_ = thread([this] {
    while (this->running_) {
      this->event_queue_->Wait();

      while (this->running_ && !this->event_queue_->Empty()) {
        Event* event = this->event_queue_->Next();

        if (event == nullptr) {
          LOG(ERROR) << "Event is nullptr";

          break;
        }

        this->description_to_event_[event->GetDescription()] = event;

        LOG(INFO) << "Scheduling event <" << event->GetDescription() << ">";

        // Event takes ownership of the timer.
        event->SetTimer(new deadline_timer(io_service_));
        event->Start();
      }
    }
  });

  LOG(INFO) << "Started event scheduler";
}

void EventScheduler::Exit() {
  LOG(INFO) << "Exiting event scheduler";

  running_ = false;
  event_queue_->Notify();

  if (main_thread_.joinable()) {
    main_thread_.join();
  }

  event_queue_->Clear();

  io_service_.stop();

  work_threads_.join_all();

  LOG(INFO) << "Exited event scheduler";
}

size_t EventScheduler::EventNum() { return description_to_event_.size(); }

const DescriptionToEvent& EventScheduler::GetDescriptionToEvent() {
  return description_to_event_;
}

}  // namespace event_scheduler
