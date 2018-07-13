package(default_visibility = ["//visibility:public"])

cc_library(
	name = "order_factory", 
	srcs = ["order_factory.cc"], 
	hdrs = ["order_factory.h"],
	deps = [
		"//interfaces/order/proto:order_proto",
	],
)

cc_test(
	name = "order_factory_test", 
	srcs = ["order_factory_test.cc"], 
	deps = [
		":order_factory",
	],
	linkopts = [
		"-lboost_unit_test_framework",
	],
	timeout = "short", 
	size = "small", 
)
