// Copyright 2016, Xuanting Cai

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include <time.h>
#include <cstdlib>

#include <boost/test/unit_test.hpp>

#include "../CHSScheduler/chs_scheduler.h"
#include "./chs_monitor.h"

using Util::setGlobalFlagForTest;

namespace Module {

struct CHSMonitorFixture {
  CHSMonitorFixture() {
    setGlobalFlagForTest();

    stats_pool.reset(new CHSStatsPool());
    event_queue.reset(new CHSQueue<CHSEvent*>());

    scheduler.reset(new CHSScheduler(stats_pool.get(), event_queue.get()));
    monitor.reset(new CHSMonitor(stats_pool.get(), event_queue.get()));
  }

  virtual ~CHSMonitorFixture() {}

  unique_ptr<CHSStatsPool> stats_pool;
  unique_ptr<CHSQueue<CHSEvent*>> event_queue;
  unique_ptr<CHSScheduler> scheduler;
  unique_ptr<CHSMonitor> monitor;
};

BOOST_FIXTURE_TEST_SUITE(CHSMonitorTest, CHSMonitorFixture)

BOOST_AUTO_TEST_CASE(Scheduler) {
  scheduler->init();
  scheduler->start();

  CHSClock::sleep(1000);

  monitor->init();
  monitor->start();

  for (uint32_t i = 0; i < 5; i++) {
    uint32_t seed = time(NULL);
    int32_t count = rand_r(seed) % 100;

    LOG(info) << "caixuanting count=" << count;

    stats_pool->increment("monitor.test_counter", count);
    CHSClock::sleep(60000);
  }

  scheduler->shutDown();
  monitor->shutDown();
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace Module
