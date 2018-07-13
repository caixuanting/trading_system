// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include <thread>

#include <boost/test/unit_test.hpp>

#include "utils/queue.h"

namespace util {

struct QueueFixture {
  QueueFixture() { queue.reset(new Queue<int32_t>()); }

  virtual ~QueueFixture() {}

  std::unique_ptr<Queue<int32_t>> queue;
};

BOOST_FIXTURE_TEST_SUITE(QueueTest, QueueFixture)

BOOST_AUTO_TEST_CASE(Wait) {
  std::thread wait_thread([this] {
    this->queue->Wait();
    this->queue->Insert(20);
  });

  int32_t front = queue->Next();
  BOOST_CHECK_EQUAL(0, front);
  BOOST_CHECK_EQUAL(0, queue->Size());

  queue->Notify();

  wait_thread.join();

  BOOST_CHECK_EQUAL(1, queue->Size());

  front = queue->Next();
  BOOST_CHECK_EQUAL(20, front);

  BOOST_CHECK_EQUAL(0, queue->Size());
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace util
