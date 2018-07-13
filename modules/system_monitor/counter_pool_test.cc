// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include <boost/test/unit_test.hpp>

#include "modules/system_monitor/counter_pool.h"

using std::string;
using std::unique_ptr;
	
namespace system_monitor {

const string kFistCounterName = "first_counter";

struct CounterPoolFixture {
  CounterPoolFixture() { counter_pool.reset(new CounterPool()); }

  virtual ~CounterPoolFixture() {}

  unique_ptr<CounterPool> counter_pool;
};

BOOST_FIXTURE_TEST_SUITE(CounterPoolTest, CounterPoolFixture)

BOOST_AUTO_TEST_CASE(Increment) {
	{
		Counter counter = counter_pool->GetCounter(kFistCounterName);
	
  	BOOST_CHECK_EQUAL(0, counter.GetCurrentTotal());
  	BOOST_CHECK_EQUAL(0, counter.GetPreviousTotal());		
	}
	
	counter_pool->Increment(kFistCounterName);
	
	{
		Counter counter = counter_pool->GetCounter(kFistCounterName);
	
  	BOOST_CHECK_EQUAL(1, counter.GetCurrentTotal());
  	BOOST_CHECK_EQUAL(0, counter.GetPreviousTotal());	
	}

	counter_pool->Increment(kFistCounterName, 2);
	
	{
		Counter counter = counter_pool->GetCounter(kFistCounterName);
	
  	BOOST_CHECK_EQUAL(3, counter.GetCurrentTotal());
  	BOOST_CHECK_EQUAL(0, counter.GetPreviousTotal());	
	}
	
	counter_pool->SetCounter(kFistCounterName, 0);
	
	{
		Counter counter = counter_pool->GetCounter(kFistCounterName);
	
  	BOOST_CHECK_EQUAL(0, counter.GetCurrentTotal());
  	BOOST_CHECK_EQUAL(0, counter.GetPreviousTotal());	
	}
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace Interface
