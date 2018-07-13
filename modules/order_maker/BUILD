package(default_visibility = ["//visibility:public"])

cc_library(
	name = "order_maker", 
	hdrs = ["order_maker.h"],
	srcs = ["order_maker.cc"],
	deps = [
		"//modules/order_maker/clients:ctp_future_order_client",
		"//modules/order_maker/proto:order_maker_config_proto",
		"//utils:boost_logger",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

cc_test(
	name = "order_maker_test", 
	srcs = ["order_maker_test.cc"],
	deps = [
		":order_maker",
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

