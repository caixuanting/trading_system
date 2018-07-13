package(default_visibility = ["//visibility:public"])

load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cc_proto_library")

cc_proto_library(
	name = "system_config_proto",
	protos = [
		"system_config.proto"
	],
	proto_deps = [
		"//http_server/proto:http_server_config_proto",
		"//interfaces/data/proto:security_proto",
		"//modules/account_keeper/proto:account_keeper_config_proto",
		"//modules/data_receiver/proto:data_receiver_config_proto",
		"//modules/event_scheduler/proto:event_scheduler_config_proto",
		"//modules/signal_manager/proto:signal_manager_config_proto",
		"//modules/strategy_runner/proto:strategy_runner_config_proto",
		"//modules/order_maker/proto:order_maker_config_proto",
	],
	with_grpc = True,
)