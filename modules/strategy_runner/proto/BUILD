package(default_visibility = ["//visibility:public"])

load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cc_proto_library")

cc_proto_library(
	name = "strategy_message_proto",
	protos = [
		"strategy_message.proto",
	],
	with_grpc = True, 
)

cc_proto_library(
	name = "strategy_runner_config_proto",
	protos = [
		"strategy_runner_config.proto",
	],
	proto_deps = [
		"//modules/strategy_runner/strategies/proto:strategy_config_proto",
	],
	with_grpc = True, 
)