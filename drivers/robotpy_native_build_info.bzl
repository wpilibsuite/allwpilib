# THIS FILE IS AUTO GENERATED

load("@bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "generate_native_files", "robotpy_library")

def define_native_wrapper(name, pyproject_toml = None):
    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = [] + ["//drivers:generated-native-include-files"],
        out = "native/wpilib_drivers/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "drivers/src/generated/main/native/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    libinit_files = ["native/wpilib_drivers/_init_robotpy_native_wpilib_drivers.py"]

    generate_native_files(
        name = name,
        pyproject_toml = pyproject_toml,
        pc_deps = [
            "//wpilibc:native/wpilib/robotpy-native-wpilib.pc",
        ],
        libinit_files = libinit_files,
        pc_files = ["native/wpilib_drivers/robotpy-native-wpilib-drivers.pc"],
    )

    copy_native_file(
        name = "drivers",
        library = "shared/drivers",
        base_path = "native/wpilib_drivers/",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-native-wpilib-drivers",
        srcs = libinit_files,
        data = [
            name + ".pc_wrapper",
            ":drivers.copy_lib",
            "{}.copy_headers".format(name),
        ],
        deps = [
            "//wpilibc:robotpy-native-wpilib",
        ],
        summary = "WPILib third-party drivers library",
        requires = ["robotpy-native-wpilib==0.0.0"],
        python_requires = ">=3.11",
        strip_path_prefixes = ["drivers"],
        entry_points = {
            "pkg_config": [
                "wpilib_drivers = native.wpilib_drivers",
            ],
        },
    )
