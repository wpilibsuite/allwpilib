# THIS FILE IS AUTO GENERATED

load("@aspect_bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "native_wrappery_library")

def define_native_wrapper(name, pyproject_toml = None):
    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"]) + native.glob(["src/generated/main/native/include/**"], allow_empty = True),
        out = "native/romi/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "romiVendordep/src/generated/main/native/include": "",
            "romiVendordep/src/main/native/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    native_wrappery_library(
        name = name,
        pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml",
        libinit_file = "native/romi/_init_robotpy_native_romi.py",
        pc_file = "native/romi/robotpy-native-romi.pc",
        pc_deps = [
            "//wpilibc:native/wpilib/robotpy-native-wpilib.pc",
        ],
        deps = [
            "//wpilibc:robotpy-native-wpilib",
        ],
        headers = "{}.copy_headers".format(name),
        native_shared_library = "shared/romiVendordep",
        install_path = "native/romi/",
    )
