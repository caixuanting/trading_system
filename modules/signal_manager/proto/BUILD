package(default_visibility = ["//visibility:public"])

load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cc_proto_library")

cc_proto_library(
	name = "signal_manager_config_proto",
	protos = [
		"signal_manager_config.proto",
	],
	proto_deps = [
		"//interfaces/signal/proto:signal_config_proto",
		"//utils/proto:database_config_proto",
	],
	with_grpc = True, 
)

cc_proto_library(
	name = "signal_message_proto",
	protos = [
		"signal_message.proto",
	],
	with_grpc = True, 
)