load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "jni",
    visibility = ["//visibility:public"],
    deps = select({
        "@bazel_tools//src/conditions:darwin": ["@bazel_tools//tools/jdk:jni"],
        "@bazel_tools//src/conditions:linux_x86_64": ["@bazel_tools//tools/jdk:jni"],
        "@bazel_tools//src/conditions:windows": ["@bazel_tools//tools/jdk:jni"],
    }),
)
