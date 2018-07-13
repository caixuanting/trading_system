package(default_visibility = ["//visibility:public"])

cc_library(
	name = "http_client", 
	srcs = ["http_client.cc"], 
	hdrs = ["http_client.h"],
	deps = [
	],
	copts = [
		"-DBOOST_ALL_DYN_LINK",
		"-DBOOST_SPIRIT_USE_PHOENIX_V3",
	],
)

cc_test(
	name = "http_client_test", 
	srcs = ["http_client_test.cc"], 
	deps = [
		":http_client",
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
