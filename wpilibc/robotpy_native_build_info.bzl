# THIS FILE IS AUTO GENERATED

load("@aspect_bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "generate_native_files", "robotpy_library")

def define_native_wrapper(name, pyproject_toml = None):
    pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml"

    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"]) + native.glob(["src/generated/main/native/include/**"], allow_empty = True),
        out = "native/wpilib/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "wpilibc/src/generated/main/native/include": "",
            "wpilibc/src/main/native/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    libinit_files = ["native/wpilib/_init_robotpy_native_wpilib.py"]

    generate_native_files(
        name = name,
        pyproject_toml = pyproject_toml,
        pc_deps = [
            "//ntcore:native/ntcore/robotpy-native-ntcore.pc",
            "//hal:native/wpihal/robotpy-native-wpihal.pc",
            "//wpimath:native/wpimath/robotpy-native-wpimath.pc",
            "//wpinet:native/wpinet/robotpy-native-wpinet.pc",
            "//wpiutil:native/wpiutil/robotpy-native-wpiutil.pc",
        ],
        libinit_files = libinit_files,
        pc_files = ["native/wpilib/robotpy-native-wpilib.pc"],
    )

    copy_native_file(
        name = "wpilibc",
        library = "shared/wpilibc",
        base_path = "native/wpilib/",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-native-wpilib",
        srcs = libinit_files,
        data = [
            name + ".pc_wrapper",
            ":wpilibc.copy_lib",
            "{}.copy_headers".format(name),
        ],
        deps = [
            "//ntcore:robotpy-native-ntcore",
            "//hal:robotpy-native-wpihal",
            "//wpimath:robotpy-native-wpimath",
            "//wpinet:robotpy-native-wpinet",
            "//wpiutil:robotpy-native-wpiutil",
        ],
        summary = "WPILib Robotics Library",
        requires = ["robotpy-native-wpiutil==0.0.0", "robotpy-native-wpinet==0.0.0", "robotpy-native-ntcore==0.0.0", "robotpy-native-wpimath==0.0.0", "robotpy-native-wpihal==0.0.0"],
        python_requires = ">=3.11",
        strip_path_prefixes = ["wpilibc"],
        entry_points = {
            "pkg_config": [
                "wpilib = native.wpilib",
            ],
        },
    )
