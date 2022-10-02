load("//shared/bazel/rules:cc_rules.bzl", "wpilib_cc_library")

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
            deps = ["//wpigui/src/main/native:wpigui", "//cscore/src/main/native:cscore.static"],
            tags = [
                "wpi-example",
                "no-bionic",
                "no-raspbian",
                "no-roborio",
            ],
        )
