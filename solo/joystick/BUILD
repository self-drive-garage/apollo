load("//tools:apollo_package.bzl", "apollo_cc_library", "apollo_cc_test", "apollo_component", "apollo_package")
load("//tools:cpplint.bzl", "cpplint")

apollo_component(
    name = "libJoystickComponent.so",
    srcs = ["JoystickComponent.cpp"],
    hdrs = ["JoystickComponent.hpp"],
    visibility = ["//visibility:private"],
    deps = [
        "//cyber",
        "//modules/common_msgs/control_msgs:control_cmd_cc_proto",
        "//modules/common_msgs/guardian_msgs:guardian_cc_proto",
        "//modules/common/adapters:adapter_gflags",
        "//modules/common_msgs/guardian_msgs:guardian_cc_proto",
        "//modules/guardian/proto:guardian_conf_cc_proto",
        "//modules/common_msgs/monitor_msgs:system_status_cc_proto",
    ],
)

#apollo_cc_library(
#    name = "Joystick",
#    srcs = [
#        "Joystick.cpp",
#    ],
#    hdrs = [
#        "Joystick.hpp",
#    ],
#    deps = [
#        "//cyber",
#        "//modules/common_msgs/control_msgs:control_cmd_cc_proto",
#        "//modules/guardian:libguardian_component.so",
#        "@com_google_protobuf//:protobuf",
#    ],
#)
apollo_package()

cpplint()
