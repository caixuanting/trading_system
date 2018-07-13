package(default_visibility = ["//visibility:public"])

proto_library(
	name = "time_interval_proto", 
	srcs = ["time_interval.proto"],
)

cc_proto_library(
	name = "time_interval_cc_proto",
	deps = [
	  ":time_interval_proto",
	],
)

proto_library(
	name = "database_config_proto", 
	srcs = ["database_config.proto"],
)

cc_proto_library(
	name = "database_config_cc_proto",
	deps = [
	  ":database_config_proto",
	],
)