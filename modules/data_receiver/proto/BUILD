package(default_visibility = ["//visibility:public"])

load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cc_proto_library")

cc_proto_library(
	name = "data_receiver_config_proto",
	protos = [
		"data_receiver_config.proto",
	],
	proto_deps = [
		"//modules/data_receiver/clients/proto:ctp_future_data_client_config_proto",
		"//utils/proto:database_config_proto",
	],
	with_grpc = True, 
)

cc_proto_library(
	name = "data_message_proto",
	protos = [
		"data_message.proto",
	],
	with_grpc = True, 
)