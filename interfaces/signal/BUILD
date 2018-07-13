package(default_visibility = ["//visibility:public"])

cc_library(
	name = "signal_base", 
	srcs = ["signal_base.cc"], 
	hdrs = ["signal_base.h"],
	deps = [
		"//interfaces/data/proto:security_proto",
		"//interfaces/signal/proto:signal_proto",
		"//utils/proto:database_config_proto",
	],
)

cc_library(
	name = "signal_simple", 
	srcs = ["signal_simple.cc"], 
	hdrs = ["signal_simple.h"],
	deps = [
		":signal_base",
		"//interfaces/data:data",
	],
)

cc_test(
	name = "signal_simple_test", 
	srcs = ["signal_simple_test.cc"], 
	deps = [
		":signal_simple",
	],
	linkopts = [
		"-lboost_date_time",
		"-lboost_unit_test_framework",
	],
	timeout = "short", 
	size = "small", 
)

cc_library(
	name = "signal_periodic", 
	srcs = ["signal_periodic.cc"], 
	hdrs = ["signal_periodic.h"],
	deps = [
		":signal_base",
		"//utils:boost_logger",
		"//utils:database_operator",
		"//utils:date_time",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	], 
)

cc_test(
	name = "signal_periodic_test", 
	srcs = ["signal_periodic_test.cc"], 
	deps = [
		":signal_periodic",
		":signal_simple",
	],
	linkopts = [
		"-lboost_date_time",
		"-lboost_unit_test_framework",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
	timeout = "short", 
	size = "small", 
)

cc_library(
	name = "signal_moving_average", 
	srcs = ["signal_moving_average.cc"], 
	hdrs = ["signal_moving_average.h"],
	deps = [
		":signal_periodic",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	], 
)

cc_test(
	name = "signal_moving_average_test", 
	srcs = ["signal_moving_average_test.cc"], 
	deps = [
		":signal_moving_average",
		":signal_simple",
	],
	linkopts = [
		"-lboost_date_time",
		"-lboost_unit_test_framework",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
	timeout = "short", 
	size = "small", 
)

cc_library(
	name = "signal_pool", 
	srcs = ["signal_pool.cc"], 
	hdrs = ["signal_pool.h"],
	deps = [
		"//interfaces/data:data_pool",
		"//interfaces/signal/proto:signal_config_proto",
		"//utils:graph",
		":signal_moving_average",
		":signal_simple",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	], 
)

cc_test(
	name = "signal_pool_test", 
	srcs = ["signal_pool_test.cc"], 
	deps = [
		":signal_pool",
	],
	linkopts = [
		"-lboost_date_time",
		"-lboost_unit_test_framework",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
	timeout = "short", 
	size = "small", 
)