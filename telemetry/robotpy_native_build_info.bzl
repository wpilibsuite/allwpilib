# THIS FILE IS AUTO GENERATED

load("@aspect_bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "generate_native_files", "robotpy_library")

def define_native_wrapper(name, pyproject_toml = None):
    pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml"

    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"]) + native.glob(["src/generated/main/native/include/**"], allow_empty = True),
        out = "native/telemetry/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "telemetry/src/generated/main/native/include": "",
            "telemetry/src/main/native/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    libinit_files = ["native/telemetry/_init_robotpy_native_telemetry.py"]

    generate_native_files(
        name = name,
        pyproject_toml = pyproject_toml,
        pc_deps = [
            "//wpiutil:native/wpiutil/robotpy-native-wpiutil.pc",
        ],
        libinit_files = libinit_files,
        pc_files = ["native/telemetry/robotpy-native-telemetry.pc"],
    )

    copy_native_file(
        name = "telemetry",
        library = "shared/telemetry",
        base_path = "native/telemetry/",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-native-telemetry",
        srcs = libinit_files,
        data = [
            name + ".pc_wrapper",
            ":telemetry.copy_lib",
            "{}.copy_headers".format(name),
        ],
        deps = [
            "//wpiutil:robotpy-native-wpiutil",
        ],
        summary = "WPILib Telemetry Library",
        requires = ["robotpy-native-wpiutil==0.0.0"],
        python_requires = "",
        strip_path_prefixes = ["telemetry"],
        entry_points = {
            "pkg_config": [
                "telemetry = native.telemetry",
            ],
        },
    )
