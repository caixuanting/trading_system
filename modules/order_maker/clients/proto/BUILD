package(default_visibility = ["//visibility:public"])

load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cc_proto_library")

cc_proto_library(
	name = "ctp_future_order_client_config_proto",
	protos = [
		"ctp_future_order_client_config.proto",
	],
	proto_deps = [
		"//utils/proto:database_config_proto",
		"//utils/proto:time_interval_proto",
	],
	with_grpc = True, 
)

