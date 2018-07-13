package(default_visibility = ["//visibility:public"])

load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cc_proto_library")

cc_proto_library(
	name = "account_keeper_config_proto",
	protos = [
		"account_keeper_config.proto",
	],
	proto_deps = [
		"//utils/proto:database_config_proto",
	],
	with_grpc = True, 
)

cc_proto_library(
	name = "account_message_proto",
	protos = [
		"account_message.proto",
	],
	with_grpc = True, 
)
