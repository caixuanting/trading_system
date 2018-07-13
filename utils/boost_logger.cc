// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "utils/boost_logger.h"

#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

using namespace boost::log;
using namespace boost::log::expressions;
using namespace boost::log::keywords;
using namespace boost::log::sinks::file;
using namespace boost::log::trivial;

using boost::log::core;
using boost::log::sinks::file::rotation_at_time_point;
using boost::log::sinks::text_file_backend;
using boost::make_shared;
using boost::posix_time::ptime;
using boost::shared_ptr;
using std::string;

namespace util {

shared_ptr<text_file_backend> CreateSinkBackend(const string& severity,
                                                const string& log_dir) {
  return make_shared<text_file_backend>(
      keywords::file_name = log_dir + severity + "_%Y-%m-%d_%H:%M:%S",
      keywords::rotation_size = 5 * 1024 /* rotate every 5kB */,
      keywords::time_based_rotation = rotation_at_time_point(0, 0, 0));
}

shared_ptr<text_sink> CreateSinkFrontend(
    shared_ptr<text_file_backend> sink_backend) {
  shared_ptr<text_sink> sink_frontend = make_shared<text_sink>(sink_backend);
  sink_frontend->set_formatter(expressions::stream
                               << "[" << expressions::format_date_time<ptime>(
                                             "TimeStamp", "%Y-%m-%d %H:%M:%S")
                               << "]" << expressions::smessage);
  return sink_frontend;
}

void SetFilter(const trivial::severity_level severity,
               shared_ptr<text_sink> sink_frontend) {
  sink_frontend->set_filter(
      expressions::attr<trivial::severity_level>("Severity") == severity);
}

void InitBoostLogger(const string& log_dir) {
  shared_ptr<text_file_backend> info_backend =
      CreateSinkBackend("info", log_dir);
  shared_ptr<text_sink> info_frontend = CreateSinkFrontend(info_backend);
  SetFilter(trivial::info, info_frontend);
  info_backend->auto_flush(true);

  shared_ptr<text_file_backend> warning_backend =
      CreateSinkBackend("warning", log_dir);
  shared_ptr<text_sink> warning_frontend = CreateSinkFrontend(warning_backend);
  SetFilter(trivial::warning, warning_frontend);
  warning_backend->auto_flush(true);

  shared_ptr<text_file_backend> error_backend =
      CreateSinkBackend("error", log_dir);
  shared_ptr<text_sink> error_frontend = CreateSinkFrontend(error_backend);
  SetFilter(trivial::error, error_frontend);
  error_backend->auto_flush(true);

  shared_ptr<core> core = core::get();
  core->add_sink(info_frontend);
  core->add_sink(warning_frontend);
  core->add_sink(error_frontend);

  boost::log::add_common_attributes();
}

}  // namespace util
