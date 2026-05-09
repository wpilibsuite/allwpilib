# THIS FILE IS AUTO GENERATED

load("@aspect_bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "generate_native_files", "robotpy_library")

def define_native_wrapper(name, pyproject_toml = None):
    pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml"

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

    libinit_files = ["native/romi/_init_robotpy_native_romi.py"]

    generate_native_files(
        name = name,
        pyproject_toml = pyproject_toml,
        pc_deps = [
            "//wpilibc:native/wpilib/robotpy-native-wpilib.pc",
        ],
        libinit_files = libinit_files,
        pc_files = ["native/romi/robotpy-native-romi.pc"],
    )

    copy_native_file(
        name = "romiVendordep",
        library = "shared/romiVendordep",
        base_path = "native/romi/",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-native-romi",
        srcs = libinit_files,
        data = [
            name + ".pc_wrapper",
            ":romiVendordep.copy_lib",
            "{}.copy_headers".format(name),
        ],
        deps = [
            "//wpilibc:robotpy-native-wpilib",
        ],
        summary = "WPILib Romi support library",
        requires = ["robotpy-native-wpilib==0.0.0"],
        python_requires = ">=3.11",
        strip_path_prefixes = ["romiVendordep"],
        entry_points = {
            "pkg_config": [
                "romi = native.romi",
            ],
        },
    )
