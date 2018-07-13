package(default_visibility = ["//visibility:public"])

load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cc_proto_library")

cc_proto_library(
	name = "order_message_proto",
	protos = [
		"order_message.proto",
	],
	with_grpc = True, 
)

cc_proto_library(
	name = "order_maker_config_proto",
	protos = [
		"order_maker_config.proto",
	],
	proto_deps = [
		"//modules/order_maker/clients/proto:ctp_future_order_client_config_proto",
	],
	with_grpc = True, 
)