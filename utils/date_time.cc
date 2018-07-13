// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "utils/date_time.h"

#include <locale>

using boost::gregorian::to_iso_extended_string;
using boost::gregorian::date;
using boost::gregorian::days;
using boost::posix_time::ptime;
using boost::posix_time::seconds;
using boost::posix_time::time_facet;
using boost::posix_time::time_from_string;
using std::locale;
using std::string;
using std::stringstream;

namespace util {

const string kNotADateTime = "not-a-date-time";

string ToString(const ptime& date_time) {
  if (date_time.is_not_a_date_time()) {
    return kNotADateTime;
  } else {
    time_facet* facet = new time_facet();
    facet->format("%Y-%m-%d %H:%M:%s");

    stringstream string_stream;
    string_stream.imbue(locale(locale::classic(), facet));
    string_stream << date_time;
    return string_stream.str();
  }
}

bool TheSameMinute(const string& first_minute_string,
                   const string& second_minute_string) {
  ptime first_minute = time_from_string(first_minute_string);
  ptime second_minute = time_from_string(second_minute_string);

  return TheSameMinute(first_minute, second_minute);
}

bool TheSameMinute(const ptime& first_minute, const ptime& second_minute) {
  if (first_minute.is_not_a_date_time() || second_minute.is_not_a_date_time()) {
    return false;
  }

  return first_minute.date() == second_minute.date() &&
         first_minute.time_of_day().hours() ==
             second_minute.time_of_day().hours() &&
         first_minute.time_of_day().minutes() ==
             second_minute.time_of_day().minutes();
}

bool TheSameDay(const std::string& first_minute_string,
                const std::string& second_minute_string) {
  ptime first_minute = time_from_string(first_minute_string);
  ptime second_minute = time_from_string(second_minute_string);

  return TheSameDay(first_minute, second_minute);
}

bool TheSameDay(const boost::posix_time::ptime& first_minute,
                const boost::posix_time::ptime& second_minute) {
  if (first_minute.is_not_a_date_time() || second_minute.is_not_a_date_time()) {
    return false;
  }

  return first_minute.date() == second_minute.date();
}

ptime Minus(const ptime& date_time, const int32_t time_in_seconds) {
  if (date_time.is_not_a_date_time()) {
    return ptime();
  }

  return date_time - seconds(time_in_seconds);
}

ptime Minus(const string& date_time_string, const int32_t time_in_seconds) {
  ptime date_time = time_from_string(date_time_string);

  return Minus(date_time, time_in_seconds);
}

bool WithinTimeInterval(const string& date_time_string,
                        const TimeInterval& time_interval) {
  ptime begin_time = time_from_string(time_interval.begin_time());
  ptime end_time = time_from_string(time_interval.end_time());
  ptime date_time = time_from_string(date_time_string);

  if (date_time.is_not_a_date_time() || begin_time.is_not_a_date_time() ||
      end_time.is_not_a_date_time()) {
    return false;
  }

  return begin_time <= date_time && date_time <= end_time;
}

bool Within(const std::string& date_time_string,
            const std::string& end_time_string,
            const uint32_t time_in_seconds) {
  ptime date_time = time_from_string(date_time_string);
  ptime end_time = time_from_string(end_time_string);

  if (date_time.is_not_a_date_time() || end_time.is_not_a_date_time()) {
    return false;
  }

  return date_time >= end_time - seconds(time_in_seconds) &&
         date_time < end_time;
}

bool LessOrEqual(const std::string& left_date_time_string,
                 const std::string& right_date_time_string) {
  ptime left_date_time = time_from_string(left_date_time_string);
  ptime right_date_time = time_from_string(right_date_time_string);

  return left_date_time <= right_date_time;
}

TimeInterval CreateTimeInterval(const std::string& begin_time,
                                const std::string& end_time, const date& d) {
  TimeInterval time_interval;

  if (end_time < begin_time) {
    string date_string = to_iso_extended_string(d);
    time_interval.set_begin_time(date_string + " " + begin_time);
    string next_date_string = to_iso_extended_string(d + days(1));
    time_interval.set_end_time(next_date_string + " " + end_time);
  } else {
    string date_string = to_iso_extended_string(d);
    time_interval.set_begin_time(date_string + " " + begin_time);
    time_interval.set_end_time(date_string + " " + end_time);
  }

  return time_interval;
}

}  // namespace util
