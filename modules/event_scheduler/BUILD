package(default_visibility = ["//visibility:public"])

cc_library(
	name = "event", 
	hdrs = ["event.h"], 
	srcs = ["event.cc"],
	deps = [
	],
	linkopts = [
		"-lboost_date_time",
		"-lboost_system",
		"-lglog",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	], 
)

cc_test(
	name = "event_test", 
	srcs = ["event_test.cc"], 
	deps = [
		":event",
	],
	linkopts = [
		"-lboost_unit_test_framework",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	], 
)

cc_library(
	name = "event_scheduler", 
	hdrs = ["event_scheduler.h"],
	srcs = ["event_scheduler.cc"],
	deps = [
		"//modules/event_scheduler/proto:event_scheduler_config_proto",
		"//utils:queue",
		":event",
	],
	linkopts = [
		"-lboost_thread",
	],
	copts = [ 
		"-DBOOST_ALL_DYN_LINK",
	], 
)

cc_test(
	name = "event_scheduler_test",
	srcs = ["event_scheduler_test.cc"],
	deps = [
		":event_scheduler",
		"//utils:date_time",
	],
	linkopts = [
		"-lboost_unit_test_framework",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	], 
)
