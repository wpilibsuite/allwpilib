# THIS FILE IS AUTO GENERATED

load("@aspect_bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "generate_native_files", "robotpy_library")

def define_native_wrapper(name, pyproject_toml = None):
    pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml"

    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"]) + native.glob(["src/generated/main/native/include/**"], allow_empty = True),
        out = "native/ntcore/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "ntcore/src/generated/main/native/include": "",
            "ntcore/src/main/native/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    libinit_files = ["native/ntcore/_init_robotpy_native_ntcore.py"]

    generate_native_files(
        name = name,
        pyproject_toml = pyproject_toml,
        pc_deps = [
            "//datalog:native/datalog/robotpy-native-datalog.pc",
            "//wpinet:native/wpinet/robotpy-native-wpinet.pc",
            "//wpiutil:native/wpiutil/robotpy-native-wpiutil.pc",
        ],
        libinit_files = libinit_files,
        pc_files = ["native/ntcore/robotpy-native-ntcore.pc"],
    )

    copy_native_file(
        name = "ntcore",
        library = "shared/ntcore",
        base_path = "native/ntcore/",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-native-ntcore",
        srcs = libinit_files,
        data = [
            name + ".pc_wrapper",
            ":ntcore.copy_lib",
            "{}.copy_headers".format(name),
        ],
        deps = [
            "//datalog:robotpy-native-datalog",
            "//wpinet:robotpy-native-wpinet",
            "//wpiutil:robotpy-native-wpiutil",
        ],
        summary = "WPILib NetworkTables Library",
        requires = ["robotpy-native-wpiutil==0.0.0", "robotpy-native-wpinet==0.0.0", "robotpy-native-datalog==0.0.0"],
        python_requires = ">=3.11",
        strip_path_prefixes = ["ntcore"],
        entry_points = {
            "pkg_config": [
                "ntcore = native.ntcore",
            ],
        },
    )
