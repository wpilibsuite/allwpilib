load("//shared/bazel/rules/gen:gen-version-file.bzl", "generate_version_file")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "robotpy_library")

def define_pybind_library(name, pkgcfgs = [], extra_pybind_hdrs = []):
    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["halsim_ds_socket/**"], exclude = ["halsim_ds_socket/**/*.py"]),
        tags = ["manual", "robotpy"],
    )

    generate_version_file(
        name = "{}.generate_version".format(name),
        output_file = "halsim_ds_socket/version.py",
        template = "//shared/bazel/rules/robotpy:version_template.in",
        version_variable = "ROBOTPY_VERSION",
    )

    copy_native_file(
        name = "halsim_ds_socket",
        library = "//simulation/halsim_ds_socket:shared/halsim_ds_socket",
        base_path = "halsim_ds_socket/",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-halsim-ds-socket",
        srcs = native.glob(["halsim_ds_socket/**/*.py"]) + [
            "{}.generate_version".format(name),
        ],
        data = [
            "{}.extra_files".format(name),
            ":halsim_ds_socket.copy_lib",
        ],
        imports = ["."],
        deps = [
            "//hal:robotpy-native-wpihal",
            "//wpinet:robotpy-native-wpinet",
        ],
        strip_path_prefixes = ["simulation/halsim_ds_socket/src/main/python/"],
        summary = "WPILib simulator DS Socket Extension",
        project_urls = None,
        author_email = "RobotPy Development Team <robotpy@googlegroups.com>",
        requires = ["robotpy-native-wpihal==0.0.0", "robotpy-native-wpinet==0.0.0"],
        python_requires = ">=3.12",
        entry_points = {
            "robotpy_sim.2027": ["ds-socket = halsim_ds_socket"],
        },
        visibility = ["//visibility:public"],
    )
