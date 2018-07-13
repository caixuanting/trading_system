package(default_visibility = ["//visibility:public"])

cc_library(
	name = "account_operator", 
	srcs = ["account_operator.cc"], 
	hdrs = ["account_operator.h"],
	deps = [
		"//interfaces/account/proto:account_proto",
		"//utils:boost_logger",
		"//utils:date_time",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

cc_test(
	name = "account_operator_test", 
	srcs = ["account_operator_test.cc"], 
	deps = [
		":account_operator",
	],
	linkopts = [
		"-lboost_date_time",
		"-lboost_unit_test_framework",
	],
	timeout = "short", 
	size = "small", 
)

cc_library(
	name = "account_pool", 
	srcs = ["account_pool.cc"], 
	hdrs = ["account_pool.h"],
	deps = [
		":account_operator",
		"//utils:boost_logger",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

cc_test(
	name = "account_pool_test", 
	srcs = ["account_pool_test.cc"], 
	deps = [
		":account_pool",
	],
	linkopts = [
		"-lboost_unit_test_framework",
	],
	timeout = "short", 
	size = "small", 
)
