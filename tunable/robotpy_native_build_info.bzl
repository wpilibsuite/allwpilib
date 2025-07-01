# THIS FILE IS AUTO GENERATED

load("@aspect_bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "generate_native_files", "robotpy_library")

def define_native_wrapper(name, pyproject_toml = None):
    pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml"

    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"]) + native.glob(["src/generated/main/native/include/**"], allow_empty = True),
        out = "native/tunable/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "tunable/src/generated/main/native/include": "",
            "tunable/src/main/native/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    libinit_files = ["native/tunable/_init_robotpy_native_tunable.py"]

    generate_native_files(
        name = name,
        pyproject_toml = pyproject_toml,
        pc_deps = [
            "//wpiutil:native/wpiutil/robotpy-native-wpiutil.pc",
        ],
        libinit_files = libinit_files,
        pc_files = ["native/tunable/robotpy-native-tunable.pc"],
    )

    copy_native_file(
        name = "tunable",
        library = "shared/tunable",
        base_path = "native/tunable/",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-native-tunable",
        srcs = libinit_files,
        data = [
            name + ".pc_wrapper",
            ":tunable.copy_lib",
            "{}.copy_headers".format(name),
        ],
        deps = [
            "//wpiutil:robotpy-native-wpiutil",
        ],
        summary = "WPILib Tunable Library",
        requires = ["robotpy-native-wpiutil==0.0.0"],
        python_requires = "",
        strip_path_prefixes = ["tunable"],
        entry_points = {
            "pkg_config": [
                "tunable = native.tunable",
            ],
        },
    )
