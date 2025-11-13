# THIS FILE IS AUTO GENERATED

load("@aspect_bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "native_wrappery_library")

def define_native_wrapper(name, pyproject_toml = None):
    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"]) + native.glob(["src/generated/main/native/include/**"], allow_empty = True) + native.glob([
            "src/main/native/thirdparty/apriltag/include/**",
        ]),
        out = "native/apriltag/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "apriltag/src/generated/main/native/include": "",
            "apriltag/src/main/native/include": "",
            "apriltag/src/main/native/thirdparty/apriltag/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    native_wrappery_library(
        name = name,
        pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml",
        libinit_file = "native/apriltag/_init_robotpy_native_apriltag.py",
        pc_file = "native/apriltag/robotpy-native-apriltag.pc",
        pc_deps = [
            "//wpimath:native/wpimath/robotpy-native-wpimath.pc",
            "//wpiutil:native/wpiutil/robotpy-native-wpiutil.pc",
        ],
        deps = [
            "//wpimath:robotpy-native-wpimath",
            "//wpiutil:robotpy-native-wpiutil",
        ],
        headers = "{}.copy_headers".format(name),
        native_shared_library = "shared/apriltag",
        install_path = "native/apriltag/",
    )
