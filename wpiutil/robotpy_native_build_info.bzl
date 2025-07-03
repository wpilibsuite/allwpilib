# THIS FILE IS AUTO GENERATED

load("@aspect_bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "native_wrappery_library")

def define_native_wrapper(name, pyproject_toml = None):
    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"]) + native.glob(["src/generated/main/native/include/**"], allow_empty = True) + native.glob([
            "src/main/native/thirdparty/argparse/include/**",
            "src/main/native/thirdparty/debugging/include/**",
            "src/main/native/thirdparty/expected/include/**",
            "src/main/native/thirdparty/fmtlib/include/**",
            "src/main/native/thirdparty/json/include/**",
            "src/main/native/thirdparty/llvm/include/**",
            "src/main/native/thirdparty/mpack/include/**",
            "src/main/native/thirdparty/nanopb/include/**",
            "src/main/native/thirdparty/protobuf/include/**",
            "src/main/native/thirdparty/sigslot/include/**",
        ]),
        out = "native/wpiutil/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "wpiutil/src/generated/main/native/include": "",
            "wpiutil/src/main/native/include": "",
            "wpiutil/src/main/native/thirdparty/argparse/include": "",
            "wpiutil/src/main/native/thirdparty/debugging/include": "",
            "wpiutil/src/main/native/thirdparty/expected/include": "",
            "wpiutil/src/main/native/thirdparty/fmtlib/include": "",
            "wpiutil/src/main/native/thirdparty/json/include": "",
            "wpiutil/src/main/native/thirdparty/llvm/include": "",
            "wpiutil/src/main/native/thirdparty/mpack/include": "",
            "wpiutil/src/main/native/thirdparty/nanopb/include": "",
            "wpiutil/src/main/native/thirdparty/protobuf/include": "",
            "wpiutil/src/main/native/thirdparty/sigslot/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    native_wrappery_library(
        name = name,
        pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml",
        libinit_file = "native/wpiutil/_init_robotpy_native_wpiutil.py",
        pc_file = "native/wpiutil/robotpy-native-wpiutil.pc",
        pc_deps = [
        ],
        deps = [
        ],
        headers = "{}.copy_headers".format(name),
        native_shared_library = "shared/wpiutil",
        install_path = "native/wpiutil/",
        strip_path_prefixes = ["wpiutil"],
        requires = ["msvc-runtime>=14.42.34433; platform_system == 'Windows'"],
        summary = "WPILib Utility Library",
        entry_points = {
            "pkg_config": [
                "wpiutil = native.wpiutil",
            ],
        },
    )
