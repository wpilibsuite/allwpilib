# THIS FILE IS AUTO GENERATED

load("@aspect_bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "generate_native_files", "robotpy_library")

def define_native_wrapper(name, pyproject_toml = None):
    pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml"

    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"]) + native.glob(["src/generated/main/native/include/**"], allow_empty = True),
        out = "native/wpihal/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "hal/src/generated/main/native/include": "",
            "hal/src/main/native/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    libinit_files = ["native/wpihal/_init_robotpy_native_wpihal.py"]

    generate_native_files(
        name = name,
        pyproject_toml = pyproject_toml,
        pc_deps = [
            "//ntcore:native/ntcore/robotpy-native-ntcore.pc",
            "//wpiutil:native/wpiutil/robotpy-native-wpiutil.pc",
        ],
        libinit_files = libinit_files,
        pc_files = ["native/wpihal/robotpy-native-wpihal.pc"],
    )

    copy_native_file(
        name = "wpiHal",
        library = "shared/wpiHal",
        base_path = "native/wpihal/",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-native-wpihal",
        srcs = libinit_files,
        data = [
            name + ".pc_wrapper",
            ":wpiHal.copy_lib",
            "{}.copy_headers".format(name),
        ],
        deps = [
            "//ntcore:robotpy-native-ntcore",
            "//wpiutil:robotpy-native-wpiutil",
        ],
        summary = "WPILib HAL implementation",
        requires = ["robotpy-native-wpiutil==0.0.0", "robotpy-native-ntcore==0.0.0"],
        python_requires = ">=3.11",
        strip_path_prefixes = ["hal"],
        entry_points = {
            "pkg_config": [
                "wpihal = native.wpihal",
            ],
        },
    )
