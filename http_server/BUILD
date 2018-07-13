package(default_visibility = ["//visibility:public"])

cc_library(
	name = "http_server_handler", 
	srcs = ["http_server_handler.cc"], 
	hdrs = ["http_server_handler.h"],
	deps = [
		"//http_server/proto:http_server_config_proto",
		"//interfaces/data:data_pool",
		"//interfaces/signal:signal_pool",
		"//modules/event_scheduler:event",
		"//utils:date_time",
		"//utils:file",
	],
	linkopts = [
		"-lcppnetlib-client-connections",
		"-lcppnetlib-uri",
		"-lglog",
		"-ljsoncpp",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
		"-DBOOST_SPIRIT_USE_PHOENIX_V3",
	],
)

cc_library(
	name = "http_server", 
	srcs = ["http_server.cc"], 
	hdrs = ["http_server.h"],
	deps = [
		":http_server_handler",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
		"-DBOOST_SPIRIT_USE_PHOENIX_V3",
	],
)

cc_test(
	name = "http_server_test", 
	srcs = ["http_server_test.cc"], 
	deps = [
		":http_server",
		"//interfaces/data:data_pool",
	],
	linkopts = [
		"-lboost_date_time",
		"-lboost_unit_test_framework",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
		"-DBOOST_SPIRIT_USE_PHOENIX_V3",
	],
)
