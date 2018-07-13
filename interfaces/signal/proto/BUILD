package(default_visibility = ["//visibility:public"])

load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cc_proto_library")

cc_proto_library(
	name = "signal_config_proto", 
	protos = ["signal_config.proto"],
	proto_deps = [
		"//interfaces/data/proto:snapshot_proto",
		"//utils/proto:time_interval_proto",
	],
	with_grpc = True, 
)

cc_proto_library(
	name = "signal_proto",
	protos = ["signal.proto"],
	with_grpc = True,
)
