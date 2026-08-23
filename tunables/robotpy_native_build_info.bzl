# THIS FILE IS AUTO GENERATED

load("@bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "generate_native_files", "robotpy_library")

def define_native_wrapper(name, pyproject_toml = None):
    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"]),
        out = "native/tunables/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "tunables/src/main/native/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    libinit_files = ["native/tunables/_init_robotpy_native_tunables.py"]

    generate_native_files(
        name = name,
        pyproject_toml = pyproject_toml,
        pc_deps = [
            "//wpiutil:native/wpiutil/robotpy-native-wpiutil.pc",
        ],
        libinit_files = libinit_files,
        pc_files = ["native/tunables/robotpy-native-tunables.pc"],
    )

    copy_native_file(
        name = "tunables",
        library = "shared/tunables",
        base_path = "native/tunables/",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-native-tunables",
        srcs = libinit_files,
        data = [
            name + ".pc_wrapper",
            ":tunables.copy_lib",
            "{}.copy_headers".format(name),
        ],
        deps = [
            "//wpiutil:robotpy-native-wpiutil",
        ],
        summary = "WPILib Tunables Library",
        requires = ["robotpy-native-wpiutil==0.0.0"],
        python_requires = "",
        strip_path_prefixes = ["tunables"],
        entry_points = {
            "pkg_config": [
                "tunables = native.tunables",
            ],
        },
    )
