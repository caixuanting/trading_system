// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include "modules/event_scheduler/event.h"

#include <chrono>
#include <thread>

#include <boost/test/unit_test.hpp>

using boost::asio::io_service;
using boost::asio::deadline_timer;
using boost::posix_time::ptime;
using boost::posix_time::second_clock;
using boost::system::error_code;
using std::thread;
using std::this_thread::sleep_for;
using std::unique_ptr;

namespace event_scheduler {

struct EventFixture {
  EventFixture() {
    // Set up io_service.
    work.reset(new io_service::work(io));
    io_thread = thread([this] { this->io.run(); });
    timer = new deadline_timer(io);
  }

  virtual ~EventFixture() {
    io.stop();
    io_thread.join();
  }

  io_service io;
  unique_ptr<io_service::work> work;
  thread io_thread;
  deadline_timer* timer;  // Not owned.
};

BOOST_FIXTURE_TEST_SUITE(EventTests, EventFixture)

BOOST_AUTO_TEST_CASE(NonRepeatEvent) {
  int number = 0;

  Event event([&number] { number++; }, second_clock::local_time(), 0,
              "Non repeat inrement one");

  event.SetTimer(timer);

  BOOST_CHECK_EQUAL(0, number);
  BOOST_CHECK_EQUAL(false, event.GetCanceled());
  BOOST_CHECK_EQUAL(0, event.GetPeriodInSeconds());
  BOOST_CHECK_EQUAL("Non repeat inrement one", event.GetDescription());

  event.Start();

  sleep_for(std::chrono::seconds(2));

  BOOST_CHECK_EQUAL(1, number);
}

BOOST_AUTO_TEST_CASE(RepeatEvent) {
  int number = 0;

  Event event([&number] { number++; }, second_clock::local_time(), 1,
              "Repeat inrement one");

  event.SetTimer(timer);

  BOOST_CHECK_EQUAL(0, number);
  BOOST_CHECK_EQUAL(false, event.GetCanceled());
  BOOST_CHECK_EQUAL(1, event.GetPeriodInSeconds());
  BOOST_CHECK_EQUAL("Repeat inrement one", event.GetDescription());

  event.Start();

  sleep_for(std::chrono::seconds(1));

  BOOST_CHECK_EQUAL(2, number);
}

BOOST_AUTO_TEST_CASE(CancelEventBeforeStart) {
  int number = 0;

  Event event([&number] { number++; }, second_clock::local_time(), 1,
              "Cancel event before start");

  event.SetTimer(timer);

  BOOST_CHECK_EQUAL(0, number);
  BOOST_CHECK_EQUAL(false, event.GetCanceled());
  BOOST_CHECK_EQUAL(1, event.GetPeriodInSeconds());
  BOOST_CHECK_EQUAL("Cancel event before start", event.GetDescription());

  event.Cancel();

  event.Start();

  sleep_for(std::chrono::seconds(2));

  BOOST_CHECK_EQUAL(0, number);
}

BOOST_AUTO_TEST_CASE(CancelEventAfterStart) {
  int number = 0;

  Event event([&number] { number++; }, second_clock::local_time(), 1,
              "Cancel event after start");

  event.SetTimer(timer);

  BOOST_CHECK_EQUAL(0, number);
  BOOST_CHECK_EQUAL(false, event.GetCanceled());
  BOOST_CHECK_EQUAL(1, event.GetPeriodInSeconds());
  BOOST_CHECK_EQUAL("Cancel event after start", event.GetDescription());

  event.Start();

  sleep_for(std::chrono::seconds(1));

  event.Cancel();

  sleep_for(std::chrono::seconds(2));

  BOOST_CHECK_EQUAL(2, number);
}

BOOST_AUTO_TEST_CASE(NoTimer) {
  int number = 0;

  Event event([&number] { number++; }, second_clock::local_time(), 0,
              "Non repeat inrement one");

  BOOST_CHECK_EQUAL(0, number);
  BOOST_CHECK_EQUAL(false, event.GetCanceled());
  BOOST_CHECK_EQUAL(0, event.GetPeriodInSeconds());
  BOOST_CHECK_EQUAL("Non repeat inrement one", event.GetDescription());

  event.Start();

  sleep_for(std::chrono::seconds(2));

  BOOST_CHECK_EQUAL(0, number);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace event_scheduler
