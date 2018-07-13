package(default_visibility = ["//visibility:public"])

cc_library(
	name = "signal_manager", 
	hdrs = ["signal_manager.h"],
	srcs = ["signal_manager.cc"],
	deps = [
		"//interfaces/signal:signal_pool",
		"//modules/data_receiver/proto:data_message_proto",
		"//modules/event_scheduler:event",
		"//modules/signal_manager/proto:signal_manager_config_proto",
		"//modules/signal_manager/proto:signal_message_proto",
		"//utils:queue",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

cc_test(
	name = "signal_manager_test", 
	srcs = ["signal_manager_test.cc"],
	deps = [
		":signal_manager",
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

