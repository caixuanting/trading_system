package(default_visibility = ["//visibility:public"])

cc_library(
	name = "strategy_base", 
	hdrs = ["strategy_base.h"],
	srcs = ["strategy_base.cc"],
	deps = [
		"//interfaces/account:account_operator",
		"//interfaces/order:order_factory",
		"//modules/event_scheduler:event",
		"//modules/order_maker/proto:order_message_proto",
		"//modules/strategy_runner/proto:strategy_message_proto",
		"//modules/strategy_runner/strategies/proto:strategy_config_proto",
		"//utils:boost_logger",
		"//utils:date_time",
		"//utils:queue",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

cc_test(
	name = "strategy_base_test",
	srcs = ["strategy_base_test.cc"],
	deps = [
		":strategy_base",
	],
	linkopts = [
		"-lboost_unit_test_framework",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

cc_library(
	name = "strategy_test", 
	hdrs = ["strategy_test.h"],
	srcs = ["strategy_test.cc"],
	deps = [
		"//interfaces/data/proto:security_proto",
		"//interfaces/order/proto:order_proto",
		"//interfaces/signal:signal_base",
		":strategy_base",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

