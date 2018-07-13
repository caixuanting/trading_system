// Copyright 2016, Xuanting Cai
// Email:caixuanting@gmail.com

// Usage:
//   LOG(INFO) << "...";
//   LOG(ERROR) << "...";
//   LOG(WARNING) << "...";

#ifndef UTILS_BOOST_LOGGER_H_
#define UTILS_BOOST_LOGGER_H_

#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>

#define INFO info
#define ERROR error
#define WARNING warning
#define FATAL fatal
#define LOG(severity)                                                      \
  BOOST_LOG_TRIVIAL(severity) << "{" << __FILE__ << ":" << __LINE__ << "}" \
                              << std::endl

namespace util {

typedef boost::log::sinks::synchronous_sink<
    boost::log::sinks::text_file_backend>
    text_sink;

boost::shared_ptr<boost::log::sinks::text_file_backend> CreateSinkBackend(
    const std::string& severity);

boost::shared_ptr<text_sink> CreateSinkFrontend();

void SetFilter(const boost::log::trivial::severity_level severity,
               boost::shared_ptr<text_sink> sink_frontend);

void InitBoostLogger(const std::string& log_dir);

}  // namespace util

#endif  // UTILS_BOOST_LOGGER_H_
