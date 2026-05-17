# THIS FILE IS AUTO GENERATED

load("@aspect_bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "generate_native_files", "robotpy_library")

def define_native_wrapper(name, pyproject_toml = None):
    pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml"

    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"]) + native.glob(["src/generated/main/native/include/**"], allow_empty = True),
        out = "native/datalog/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "datalog/src/generated/main/native/include": "",
            "datalog/src/main/native/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    libinit_files = ["native/datalog/_init_robotpy_native_datalog.py"]

    generate_native_files(
        name = name,
        pyproject_toml = pyproject_toml,
        pc_deps = [
            "//wpiutil:native/wpiutil/robotpy-native-wpiutil.pc",
        ],
        libinit_files = libinit_files,
        pc_files = ["native/datalog/robotpy-native-datalog.pc"],
    )

    copy_native_file(
        name = "datalog",
        library = "shared/datalog",
        base_path = "native/datalog/",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-native-datalog",
        srcs = libinit_files,
        data = [
            name + ".pc_wrapper",
            ":datalog.copy_lib",
            "{}.copy_headers".format(name),
        ],
        deps = [
            "//wpiutil:robotpy-native-wpiutil",
        ],
        summary = "WPILib Utility Library",
        requires = ["robotpy-native-wpiutil==0.0.0"],
        python_requires = ">=3.11",
        strip_path_prefixes = ["datalog"],
        entry_points = {
            "pkg_config": [
                "datalog = native.datalog",
            ],
        },
    )
