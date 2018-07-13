package(default_visibility = ["//visibility:public"])

cc_library(
	name = "queue", 
	hdrs = ["queue.h"], 
	srcs = ["queue.cc"], 
)

cc_test(
	name = "queue_test", 
	srcs = ["queue_test.cc"], 
	deps = [
		":queue",
	],
	linkopts = [
		"-lboost_unit_test_framework",
	], 
)

cc_library(
	name = "boost_logger", 
	hdrs = ["boost_logger.h"],
	srcs = ["boost_logger.cc"], 
	linkopts = [
		"-lboost_log",
		"-lboost_log_setup",
		"-lboost_system",
		"-lboost_thread",
		"-lpthread",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	], 
)

cc_test(
	name = "boost_logger_test",
	srcs = ["boost_logger_test.cc"],
	deps = [":boost_logger"],
	linkopts = [
		"-lboost_unit_test_framework",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	], 
)

cc_library(
	name = "date_time", 
	hdrs = ["date_time.h"],
	srcs = ["date_time.cc"],
	deps = [
		"//utils/proto:time_interval_cc_proto",
	],
)

cc_test(
	name = "date_time_test",
	srcs = ["date_time_test.cc"],
	deps = [":date_time"], 
	linkopts = [
		"-lboost_date_time",
		"-lboost_unit_test_framework",
	],
)

cc_library(
	name = "file", 
	hdrs = ["file.h"],
	srcs = ["file.cc"],
	deps = [
	],
	linkopts = [
	  "-lglog",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	], 
)


cc_test(
	name = "file_test",
	srcs = ["file_test.cc"],
	deps = [":file"], 
	linkopts = [
		"-lboost_unit_test_framework",
	],
)

cc_library(
	name = "database_operator", 
	hdrs = ["database_operator.h"],
	srcs = ["database_operator.cc"],
	deps = [
		"//utils/proto:database_config_cc_proto",
		":boost_logger",
	],
	linkopts = [
		"-lglog",
		"-lmysqlcppconn",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	], 
)


cc_test(
	name = "database_operator_test",
	srcs = ["database_operator_test.cc"],
	deps = [":database_operator"], 
	linkopts = [
		"-lboost_unit_test_framework",
	],
)

cc_library(
	name = "proto_helper", 
	hdrs = ["proto_helper.h"],
	srcs = ["proto_helper.cc"],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
		"-Ithird_party/protobuf/src",
	], 
)

cc_test(
  name = "proto_helper_test",
  srcs = ["proto_helper_test.cc"],
  deps = [
    "//utils/proto:time_interval_cc_proto",
    ":proto_helper",
  ],
  linkopts = [
		"-lboost_unit_test_framework",
	  "-lglog",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

cc_library(
	name = "graph", 
	hdrs = ["graph.h"],
	srcs = ["graph.cc"],
)

cc_test(
  name = "graph_test",
  srcs = ["graph_test.cc"],
  deps = [
  	":graph",
 	],
 	linkopts = [
		"-lboost_unit_test_framework",
	]
)
  	