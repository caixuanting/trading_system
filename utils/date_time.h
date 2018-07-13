// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef UTILS_DATE_TIME_H_
#define UTILS_DATE_TIME_H_

#include <string>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "utils/proto/time_interval.pb.h"

namespace util {

std::string ToString(const boost::posix_time::ptime& date_time);

bool TheSameMinute(const std::string& first_minute_string,
                   const std::string& second_minute_string);

bool TheSameMinute(const boost::posix_time::ptime& first_minute,
                   const boost::posix_time::ptime& second_minute);

bool TheSameDay(const std::string& first_minute_string,
                const std::string& second_minute_string);

bool TheSameDay(const boost::posix_time::ptime& first_minute,
                const boost::posix_time::ptime& second_minute);

boost::posix_time::ptime Minus(const boost::posix_time::ptime& date_time,
                               const int32_t time_in_seconds);

boost::posix_time::ptime Minus(const std::string& date_time,
                               const int32_t time_in_seconds);

bool WithinTimeInterval(const std::string& date_time_string,
                        const TimeInterval& time_interval);

// Check whether date_time is in [end_time - time_in_secnds, end_time)
bool Within(const std::string& date_time_string,
            const std::string& end_time_string, const uint32_t time_in_seconds);

bool LessOrEqual(const std::string& left_date_time_string,
                 const std::string& right_date_time_string);

TimeInterval CreateTimeInterval(const std::string& begin_time,
                                const std::string& end_time,
                                const boost::gregorian::date& d);

}  // namespace util

#endif  // UTILS_DATE_TIME_H_
