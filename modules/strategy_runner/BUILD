package(default_visibility = ["//visibility:public"])

cc_library(
	name = "strategy_runner", 
	hdrs = ["strategy_runner.h"],
	srcs = ["strategy_runner.cc"],
	deps = [
		"//interfaces/account:account_pool",
		"//interfaces/data:data_pool",
		"//interfaces/signal:signal_pool",
		"//modules/event_scheduler:event",
		"//modules/signal_manager/proto:signal_message_proto",
		"//modules/strategy_runner/proto:strategy_runner_config_proto",
		"//modules/strategy_runner/strategies:strategy_base",
		"//modules/strategy_runner/strategies:strategy_test",
		"//utils:boost_logger",
		"//utils:queue",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

cc_test(
	name = "strategy_runner_test", 
	srcs = ["strategy_runner_test.cc"],
	deps = [
		":strategy_runner",
	],
	linkopts = [
		"-lboost_unit_test_framework",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
	timeout = "short",
	size = "small",
)

