load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "generate-java-resources",
    srcs = ["generate-java-resources.py"],
    data = [
        "FRCNetComm.java.in",
        "FRCUsageReporting.h.in",
        "Instances.txt",
        "ResourceType.txt",
    ],
    visibility = ["//visibility:public"],
)

# hal/src/generate/generate-java-resources
