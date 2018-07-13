package(default_visibility = ["//visibility:public"])

cc_library(
	name = "counter", 
	hdrs = ["counter.h"],
	srcs = ["counter.cc"],
	deps = [
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

cc_test(
	name = "counter_test", 
	srcs = ["counter_test.cc"],
	deps = [
		":counter",
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

cc_library(
	name = "counter_pool", 
	hdrs = ["counter_pool.h"],
	srcs = ["counter_pool.cc"],
	deps = [
			":counter",
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
	],
)

cc_test(
	name = "counter_pool_test", 
	srcs = ["counter_pool_test.cc"],
	deps = [
		":counter_pool",
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

