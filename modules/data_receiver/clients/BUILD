package(default_visibility = ["//visibility:public"])

cc_library(
	name = "data_client", 
	hdrs = ["data_client.h"],
	srcs = ["data_client.cc"], 
	deps = [
		"//interfaces/data:data_pool",
	], 
)

cc_library(
	name = "ctp_future_data_client",
	hdrs = ["ctp_future_data_client.h"],
	srcs = ["ctp_future_data_client.cc"],
	deps = [
		"//interfaces/data:data_pool",
		"//modules/data_receiver/clients/proto:ctp_future_data_client_config_proto",
		"//modules/data_receiver/clients:data_client",
		"//modules/data_receiver/proto:data_message_proto",
		"//modules/event_scheduler:event",
		"//utils:boost_logger",
		"//utils:date_time",
		"//utils:queue",
	],
	linkopts = [
		"-lthostmduserapi",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	], 
)

cc_test(
	name = "ctp_future_data_client_test",
	srcs = ["ctp_future_data_client_test.cc"],
	deps = [
		":ctp_future_data_client",
		":data_client",
		"//interfaces/data:data_pool",
		"//modules/data_receiver/clients/proto:ctp_future_data_client_config_proto",
	],
	linkopts = [
		"-lboost_log",
		"-lboost_log_setup",
		"-lboost_filesystem",
		"-lboost_system",
		"-lboost_thread",
		"-lboost_unit_test_framework",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	], 
)
