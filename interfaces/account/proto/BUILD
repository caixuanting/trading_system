package(default_visibility = ["//visibility:public"])

load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cc_proto_library")

cc_proto_library(
	name = "position_proto", 
	protos = ["position.proto"],
	with_grpc = True, 
)

cc_proto_library(
	name = "account_proto", 
	protos = ["account.proto"],
	proto_deps = [
		":position_proto",
		"//interfaces/order/proto:order_proto",
	],
	with_grpc = True, 
)