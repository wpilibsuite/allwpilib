# THIS FILE IS AUTO GENERATED

load("@aspect_bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "native_wrappery_library")

def define_native_wrapper(name, pyproject_toml = None):
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

    native_wrappery_library(
        name = name,
        pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml",
        libinit_file = "native/tunable/_init_robotpy_native_tunable.py",
        pc_file = "native/tunable/robotpy-native-tunable.pc",
        pc_deps = [
            "//wpiutil:native/wpiutil/robotpy-native-wpiutil.pc",
        ],
        deps = [
            "//wpiutil:robotpy-native-wpiutil",
        ],
        headers = "{}.copy_headers".format(name),
        native_shared_library = "shared/tunable",
        install_path = "native/tunable/",
    )
