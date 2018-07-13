// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include <chrono>

#include <boost/test/unit_test.hpp>

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include "modules/event_scheduler/event_scheduler.h"
#include "utils/date_time.h"

using boost::asio::deadline_timer;
using boost::asio::placeholders::error;
using boost::posix_time::second_clock;
using boost::posix_time::to_simple_string;
using boost::system::error_code;
using google::protobuf::TextFormat;
using std::endl;
using std::this_thread::sleep_for;
using std::unique_ptr;
using util::Queue;
using util::ToString;

namespace event_scheduler {

struct EventSchedulerFixture {
  EventSchedulerFixture() {
    event_queue.reset(new Queue<Event*>());

    EventSchedulerConfig config;
    TextFormat::ParseFromString(
        R"(num_threads: 2)", &config);

    event_scheduler.reset(new EventScheduler(config, event_queue.get()));
  }

  virtual ~EventSchedulerFixture() {}

  unique_ptr<Queue<Event*>> event_queue;
  unique_ptr<EventScheduler> event_scheduler;
};

BOOST_FIXTURE_TEST_SUITE(EventSchedulerTest, EventSchedulerFixture)

BOOST_AUTO_TEST_CASE(Scheduler) {
  event_scheduler->Init();
  event_scheduler->Start();

  sleep_for(std::chrono::milliseconds(100));

  int32_t number_one = 1;
  Event increment_one([&number_one]() { number_one++; },
                      second_clock::local_time(), 0,
                      "Event to increment number by one");

  event_queue->Insert(&increment_one);
  event_queue->Notify();
  sleep_for(std::chrono::milliseconds(100));

  BOOST_CHECK_EQUAL(1, event_scheduler->EventNum());

  int32_t number_two = 2;
  Event repeat_increment_one([&number_two]() { number_two++; },
                             second_clock::local_time(), 1,
                             "Event to repeatedly increment number by one");

  event_queue->Insert(&repeat_increment_one);
  event_queue->Notify();
  sleep_for(std::chrono::milliseconds(100));

  BOOST_CHECK_EQUAL(2, event_scheduler->EventNum());

  for (const auto& description_to_event :
       event_scheduler->GetDescriptionToEvent()) {
    Event* event = description_to_event.second;

    LOG(INFO) << endl
              << "Description <" << description_to_event.first << ">" << endl
              << "Canceled <" << event->GetCanceled() << ">" << endl
              << "Start time <" << ToString(event->GetStartTime()) << ">"
              << endl
              << "Expires at <" << ToString(event->GetExpiresAt()) << ">"
              << endl
              << "Expires from now <"
              << to_simple_string(event->GetExpiresFromNow()) << ">" << endl
              << "Period in seconds <" << event->GetPeriodInSeconds() << ">";
  }

  sleep_for(std::chrono::milliseconds(1100));

  repeat_increment_one.Cancel();

  sleep_for(std::chrono::milliseconds(100));

  BOOST_CHECK_EQUAL(2, number_one);
  BOOST_CHECK_EQUAL(4, number_two);

  event_scheduler->Exit();
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace Module
