load("@wpi_bazel_rules//rules:cc.bzl", "wpilib_cc_library")
load("@rules_java//java:defs.bzl", "java_library")

EXAMPLE_FOLDERS = [
    "enum_usb",
    "httpcvstream",
    "settings",
    "usbcvstream",
    "usbstream",
    "usbviewer",
]

def build_cpp_examples():
    for example_dir in EXAMPLE_FOLDERS:
        wpilib_cc_library(
            name = example_dir + "-examples",
            srcs = native.glob([
                "examples/" + example_dir + "/*.cpp",
            ]),
            raw_deps = ["//wpigui:wpigui"],
            wpi_shared_deps = [":cscore"],
            tags = [
                "wpi-example",
                "no-bionic",
                "no-raspbian",
                "no-roborio",
            ],
        )
