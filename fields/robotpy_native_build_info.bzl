# THIS FILE IS AUTO GENERATED

load("@bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "generate_native_files", "robotpy_library")

def define_native_wrapper(name, pyproject_toml = None):
    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"]) + ["//fields:generated-native-include-files"],
        out = "native/fields/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "fields/src/generated/main/native/include": "",
            "fields/src/main/native/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    libinit_files = ["native/fields/_init_robotpy_native_fields.py"]

    generate_native_files(
        name = name,
        pyproject_toml = pyproject_toml,
        pc_deps = [
            "//wpimath:native/wpimath/robotpy-native-wpimath.pc",
            "//wpiutil:native/wpiutil/robotpy-native-wpiutil.pc",
        ],
        libinit_files = libinit_files,
        pc_files = ["native/fields/robotpy-native-fields.pc"],
    )

    copy_native_file(
        name = "fields",
        library = "shared/fields",
        base_path = "native/fields/",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-native-fields",
        srcs = libinit_files,
        data = [
            name + ".pc_wrapper",
            ":fields.copy_lib",
            "{}.copy_headers".format(name),
        ],
        deps = [
            "//wpimath:robotpy-native-wpimath",
            "//wpiutil:robotpy-native-wpiutil",
        ],
        summary = "WPILib Fields Library",
        requires = ["robotpy-native-wpiutil==0.0.0", "robotpy-native-wpimath==0.0.0"],
        python_requires = ">=3.11",
        strip_path_prefixes = ["fields"],
        entry_points = {
            "pkg_config": [
                "fields = native.fields",
            ],
        },
    )
