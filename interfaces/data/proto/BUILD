package(default_visibility = ["//visibility:public"])

proto_library(
	name = "security_proto", 
	srcs = ["security.proto"],
	deps = [
		"//utils/proto:time_interval_proto",
	],
)

cc_proto_library(
	name = "security_cc_proto",
	deps = [
	  ":security_proto"
	],
)

proto_library(
	name = "snapshot_proto",
	srcs = ["snapshot.proto"],
)

cc_proto_library(
	name = "snapshot_cc_proto",
	deps = [
	  ":snapshot_proto"
	],
)