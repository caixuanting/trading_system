package(default_visibility = ["//visibility:public"])

cc_library(
	name = "data", 
	srcs = ["data.cc"], 
	hdrs = ["data.h"],
	deps = [
		"//interfaces/data/proto:security_cc_proto",
		"//interfaces/data/proto:snapshot_cc_proto",
		"//utils:database_operator",
		"//utils:date_time",
		"//utils:file",
	],
	linkopts = [
		"-lboost_filesystem",
		"-lglog",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

cc_test(
	name = "data_test", 
	srcs = ["data_test.cc"], 
	deps = [
		":data",
	],
	linkopts = [
		"-lboost_date_time",
		"-lboost_system",
		"-lboost_thread",
		"-lboost_unit_test_framework",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
	timeout = "short", 
	size = "small", 
)

cc_library(
	name = "data_pool", 
	srcs = ["data_pool.cc"],
	hdrs = ["data_pool.h"], 
	deps = [
		":data",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	], 
)

cc_test(
	name = "data_pool_test", 
	srcs = ["data_pool_test.cc"],
	deps = [
		":data_pool",
	],
	linkopts = [
		"-lboost_date_time",
		"-lboost_log",
		"-lboost_log_setup",
		"-lboost_system",
		"-lboost_thread",
		"-lboost_unit_test_framework",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
	timeout = "short", 
	size = "small", 
)