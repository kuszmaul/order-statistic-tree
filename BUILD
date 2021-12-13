cc_library(
    name = "raw_order_statistic_set",
    hdrs = ["raw_order_statistic_set.h"],
    deps = [
        "@com_github_google_glog//:glog",
        "@absl//absl/memory",
    ],
)

cc_library(
    name = "raw_order_statistic_map",
    hdrs = ["raw_order_statistic_map.h"],
    deps = [
        ":raw_order_statistic_set",
    ],
)

cc_library(
    name = "order_statistic_set",
    hdrs = ["order_statistic_set.h"],
    deps = [
        ":raw_order_statistic_set",
    ],
)

cc_library(
    name = "order_statistic_test_common",
    testonly = 1,
    hdrs = ["order_statistic_test_common.h"],
    deps = [
        ":raw_order_statistic_set",
        "@com_github_google_glog//:glog", #  "//base:logging",
        "@gtest//:gtest",
        #"@gtest//testing/base/public:gunit_for_library_testonly",
        "@absl//absl/random",
        "@absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "order_statistic_set_test",
    size = "small",
    srcs = ["order_statistic_set_test.cc"],
    tags = ["requires-net:loopback"],
    deps = [
        ":order_statistic_set",
        ":order_statistic_test_common",
        "@com_github_google_glog//:glog", # "//base:logging",
        "@gtest//:gtest",
        #"//testing/base/public:gunit",
        #"//testing/base/public:gunit_main",
        "@absl//absl/random",
        "@absl//absl/strings",
    ],
)
