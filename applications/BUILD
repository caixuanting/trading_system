package(default_visibility = ["//visibility:public"])

cc_binary(
	name = "data_recorder",
	srcs = ["data_recorder.cc"], 
	deps = [
		"//modules/data_receiver:data_receiver",
		"//modules/event_scheduler:event_scheduler",
		"//proto:system_config_proto",
		"//utils:file",
	  "//http_server:http_server",
	],
	linkopts = [
	  "-lglog",
	  "-lgflags",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
		"-DBOOST_SPIRIT_USE_PHOENIX_V3",
	],
)

cc_binary(
	name = "signal_recorder",
	srcs = ["signal_recorder.cc"], 
	deps = [
		"//modules/data_receiver:data_receiver",
		"//modules/event_scheduler:event_scheduler",
		"//modules/signal_manager:signal_manager",
		"//proto:system_config_proto",
		"//utils:file",
	  "//http_server:http_server",
	],
	linkopts = [
	  "-lglog",
	  "-lgflags",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
		"-DBOOST_SPIRIT_USE_PHOENIX_V3",
	],
)

cc_binary(
	name = "update_security",
	srcs = ["update_security.cc"], 
	deps = [
		"//modules/order_maker:order_maker",
		"//proto:system_config_proto",
		"//utils:boost_logger",
		"//utils:file",
	  "//modules/event_scheduler:event_scheduler",
	],
	linkopts = [
	  "-lglog",
	  "-lgflags",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
		"-DBOOST_SPIRIT_USE_PHOENIX_V3",
	],
)

cc_binary(
	name = "trading_system",
	srcs = ["trading_system.cc"], 
	deps = [
		"//http_server:http_server",
		"//modules/account_keeper:account_keeper",
		"//modules/data_receiver:data_receiver",
		"//modules/event_scheduler:event_scheduler",
		"//modules/order_maker/proto:order_message_proto",
		"//modules/order_maker:order_maker",
		"//modules/signal_manager:signal_manager",
		"//modules/strategy_runner:strategy_runner",
		"//proto:system_config_proto",
	],
	linkopts = [
	  "-lglog",
	  "-lgflags",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
		"-DBOOST_SPIRIT_USE_PHOENIX_V3",
	],
)

