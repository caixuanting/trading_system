package(default_visibility = ["//visibility:public"])

cc_library(
	name = "account_keeper", 
	hdrs = ["account_keeper.h"],
	srcs = ["account_keeper.cc"],
	deps = [
	  "//interfaces/account:account_pool",
	  "//modules/account_keeper/proto:account_keeper_config_proto",
	  "//modules/account_keeper/proto:account_message_proto",
	  "//utils:boost_logger",
	  "//utils:database_operator",
	  "//utils:queue",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

cc_test(
	name = "account_keeper_test", 
	srcs = ["account_keeper_test.cc"],
	deps = [
		":account_keeper",
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
