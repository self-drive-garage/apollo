load("//tools:apollo_package.bzl", "apollo_cc_library", "apollo_package")
load("//tools:cpplint.bzl", "cpplint")

apollo_cc_library(
    name = "apollo_statistics",
    srcs = ["statistics.cc"],
    hdrs = ["statistics.h"],
    linkopts = [
        "-L/usr/local/lib",  # Add the library search path
        "-lbvar",
    ],
    deps = [
        "//cyber/common:cyber_common",
        "//cyber/proto:role_attributes_cc_proto",
        "//cyber/time:cyber_time",
        "@com_github_gflags_gflags//:gflags",
    ],
)

apollo_package()

cpplint()
