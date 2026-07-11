# THIS FILE IS AUTO GENERATED

load("@bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "generate_native_files", "robotpy_library")

def define_native_wrapper(name, pyproject_toml = None):
    pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml"

    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"]) + native.glob(["src/generated/main/native/include/**"], allow_empty = True),
        out = "native/halsim_gui/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "simulation/halsim_gui/src/generated/main/native/include": "",
            "simulation/halsim_gui/src/main/native/include": "",
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    libinit_files = ["native/halsim_gui/_init_robotpy_native_halsim_gui.py"]

    generate_native_files(
        name = name,
        pyproject_toml = pyproject_toml,
        pc_deps = [
            "//ntcore:native/ntcore/robotpy-native-ntcore.pc",
            "//hal:native/wpihal/robotpy-native-wpihal.pc",
            "//wpimath:native/wpimath/robotpy-native-wpimath.pc",
        ],
        libinit_files = libinit_files,
        pc_files = ["native/halsim_gui/robotpy-native-halsim-gui.pc"],
    )

    copy_native_file(
        name = "halsim_gui",
        library = "shared/halsim_gui",
        base_path = "native/halsim_gui/",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-native-halsim-gui",
        srcs = libinit_files,
        data = [
            name + ".pc_wrapper",
            ":halsim_gui.copy_lib",
            "{}.copy_headers".format(name),
        ],
        deps = [
            "//ntcore:robotpy-native-ntcore",
            "//hal:robotpy-native-wpihal",
            "//wpimath:robotpy-native-wpimath",
        ],
        summary = "WPILib HALSim GUI native library",
        requires = ["robotpy-native-wpihal==0.0.0", "robotpy-native-wpimath==0.0.0", "robotpy-native-ntcore==0.0.0"],
        python_requires = ">=3.11",
        strip_path_prefixes = ["simulation/halsim_gui"],
        entry_points = {
            "pkg_config": [
                "halsim_gui = native.halsim_gui",
            ],
        },
    )
