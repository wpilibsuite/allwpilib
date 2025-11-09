# THIS FILE IS AUTO GENERATED

load("@aspect_bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "native_wrappery_library")

def define_native_wrapper(name, pyproject_toml = None):
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

    native_wrappery_library(
        name = name,
        pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml",
        libinit_file = "native/ntcore/_init_robotpy_native_ntcore.py",
        pc_file = "native/ntcore/robotpy-native-ntcore.pc",
        pc_deps = [
            "//datalog:native/datalog/robotpy-native-datalog.pc",
            "//wpinet:native/wpinet/robotpy-native-wpinet.pc",
            "//wpiutil:native/wpiutil/robotpy-native-wpiutil.pc",
        ],
        deps = [
            "//datalog:robotpy-native-datalog",
            "//wpinet:robotpy-native-wpinet",
            "//wpiutil:robotpy-native-wpiutil",
        ],
        headers = "{}.copy_headers".format(name),
        native_shared_library = "shared/ntcore",
        install_path = "native/ntcore/",
    )
