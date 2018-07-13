package(default_visibility = ["//visibility:public"])

cc_library(
	name = "data_receiver", 
	hdrs = ["data_receiver.h"],
	srcs = ["data_receiver.cc"],
	deps = [
		"//modules/data_receiver/proto:data_receiver_config_proto",
		"//modules/data_receiver/clients:ctp_future_data_client",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

cc_test(
	name = "data_receiver_test", 
	srcs = ["data_receiver_test.cc"],
	deps = [
		":data_receiver",
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
