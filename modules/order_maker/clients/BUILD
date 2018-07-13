package(default_visibility = ["//visibility:public"])

cc_library(
	name = "order_client", 
	hdrs = ["order_client.h"],
	srcs = ["order_client.cc"],
)

cc_library(
	name = "ctp_future_order_client", 
	hdrs = ["ctp_future_order_client.h"],
	srcs = ["ctp_future_order_client.cc"],
	deps = [
		"//interfaces/account:account_pool",
		"//modules/event_scheduler:event",
		"//modules/order_maker/clients/proto:ctp_future_order_client_config_proto",
		"//modules/order_maker/proto:order_message_proto",
		"//modules/strategy_runner/proto:strategy_message_proto",
		"//utils:boost_logger",
		"//utils:database_operator",
		"//utils:queue",
		":order_client",
	],
	linkopts = [
		"-lboost_filesystem",
		"-lboost_system",
		"-lthosttraderapi",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

cc_test(
	name = "ctp_future_order_client_test", 
	srcs = ["ctp_future_order_client_test.cc"],
	deps = [
		":ctp_future_order_client",
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

