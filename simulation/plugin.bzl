load("@wpi_bazel_rules//rules:cc.bzl", "default_wpilib_cc_dev_main", "wpilib_cc_shared_library", "wpilib_cc_test")

def wpilib_hal_plugin(name, test_deps = [], linkopts = [], has_test = False, **kwargs):
    wpilib_cc_shared_library(
        name,
        linkopts = linkopts,
        visibility = ["//visibility:public"],
        tags = ["no-roborio", "no-bionic", "no-raspbian"],
        **kwargs
    )

    default_wpilib_cc_dev_main(
        name = "DevMainCpp",
        wpi_shared_deps = [":" + name],
        tags = ["no-roborio", "no-bionic", "no-raspbian"],
        linkopts = linkopts,
    )

    if has_test:
        wpilib_cc_test(
            name = "test",
            srcs = native.glob(["src/test/native/cpp/*.cpp"]),
            wpi_shared_deps = [":" + name],
            linkopts = linkopts,
            raw_deps = test_deps,
        )
