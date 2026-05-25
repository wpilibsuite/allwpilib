# THIS FILE IS AUTO GENERATED

load("@aspect_bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "generate_native_files", "robotpy_library")

def define_native_wrapper(name, pyproject_toml = None):
    pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml"

    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"]) + native.glob(["src/generated/main/native/include/**"], allow_empty = True) + native.glob([
            "src/main/native/thirdparty/argparse/include/**",
            "src/main/native/thirdparty/debugging/include/**",
            "src/main/native/thirdparty/double-conversion/include/**",
            "src/main/native/thirdparty/expected/include/**",
            "src/main/native/thirdparty/fmtlib/include/**",
            "src/main/native/thirdparty/json/include/**",
            "src/main/native/thirdparty/llvm/include/**",
            "src/main/native/thirdparty/mpack/include/**",
            "src/main/native/thirdparty/nanopb/include/**",
            "src/main/native/thirdparty/sigslot/include/**",
            "src/main/native/thirdparty/upb/include/**",
        ]),
        out = "native/wpiutil/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "wpiutil/src/generated/main/native/include": "",
            "wpiutil/src/main/native/include": "",
            "wpiutil/src/main/native/thirdparty/argparse/include": "",
            "wpiutil/src/main/native/thirdparty/debugging/include": "",
            "wpiutil/src/main/native/thirdparty/double-conversion/include": "",
            "wpiutil/src/main/native/thirdparty/expected/include": "",
            "wpiutil/src/main/native/thirdparty/fmtlib/include": "",
            "wpiutil/src/main/native/thirdparty/json/include": "",
            "wpiutil/src/main/native/thirdparty/llvm/include": "",
            "wpiutil/src/main/native/thirdparty/mpack/include": "",
            "wpiutil/src/main/native/thirdparty/nanopb/include": "",
            "wpiutil/src/main/native/thirdparty/sigslot/include": "",
            "wpiutil/src/main/native/thirdparty/upb/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    libinit_files = ["native/wpiutil/_init_robotpy_native_wpiutil.py"]

    generate_native_files(
        name = name,
        pyproject_toml = pyproject_toml,
        pc_deps = [
        ],
        libinit_files = libinit_files,
        pc_files = ["native/wpiutil/robotpy-native-wpiutil.pc"],
    )

    copy_native_file(
        name = "wpiutil",
        library = "shared/wpiutil",
        base_path = "native/wpiutil/",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-native-wpiutil",
        srcs = libinit_files,
        data = [
            name + ".pc_wrapper",
            ":wpiutil.copy_lib",
            "{}.copy_headers".format(name),
        ],
        deps = [
        ],
        summary = "WPILib Utility Library",
        requires = ["msvc-runtime>=14.42.34433; platform_system == 'Windows'"],
        python_requires = ">=3.11",
        strip_path_prefixes = ["wpiutil"],
        entry_points = {
            "pkg_config": [
                "wpiutil = native.wpiutil",
            ],
        },
    )
