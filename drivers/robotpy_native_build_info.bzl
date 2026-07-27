# THIS FILE IS AUTO GENERATED

load("@bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "generate_native_files", "robotpy_library")

def define_native_wrapper(name, pyproject_toml = None):
    pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml"

    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"], allow_empty = True) + native.glob(["src/generated/main/native/include/**"], allow_empty = True),
        out = "native/drivers/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "drivers/src/generated/main/native/include": "",
            "drivers/src/main/native/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    libinit_files = ["native/drivers/_init_robotpy_native_drivers.py"]

    generate_native_files(
        name = name,
        pyproject_toml = pyproject_toml,
        pc_deps = [
            "//wpilibc:native/wpilib/robotpy-native-wpilib.pc",
        ],
        libinit_files = libinit_files,
        pc_files = ["native/drivers/robotpy-native-drivers.pc"],
    )

    copy_native_file(
        name = "drivers",
        library = "shared/drivers",
        base_path = "native/drivers/",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-native-drivers",
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
                "drivers = native.drivers",
            ],
        },
    )
