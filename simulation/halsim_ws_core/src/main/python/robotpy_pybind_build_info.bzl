# THIS FILE IS AUTO GENERATED

load("//shared/bazel/rules/gen:gen-version-file.bzl", "generate_version_file")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "robotpy_library")

def define_pybind_library(name, pkgcfgs = [], extra_pybind_hdrs = []):
    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["halsim_ws/**"], exclude = ["halsim_ws/**/*.py"]),
        tags = ["manual", "robotpy"],
    )

    generate_version_file(
        name = "{}.generate_version".format(name),
        output_file = "halsim_ws/version.py",
        template = "//shared/bazel/rules/robotpy:version_template.in",
        version_variable = "ROBOTPY_VERSION",
    )

    copy_native_file(
        name = "halsim_ws_server",
        base_path = "halsim_ws/server/",
        library = "//simulation/halsim_ws_server:shared/halsim_ws_server",
    )

    copy_native_file(
        name = "halsim_ws_client",
        base_path = "halsim_ws/client/",
        library = "//simulation/halsim_ws_client:shared/halsim_ws_client",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-halsim-ws",
        srcs = native.glob(["halsim_ws/**/*.py"]) + [
            "{}.generate_version".format(name),
        ],
        data = [
            "{}.extra_files".format(name),
            ":halsim_ws_server.copy_lib",
            ":halsim_ws_client.copy_lib",
        ],
        imports = ["."],
        deps = [
            "//hal:robotpy-native-wpihal",
            "//wpinet:robotpy-native-wpinet",
        ],
        strip_path_prefixes = ["simulation/halsim_ws_core/src/main/python/"],
        summary = "WPILib simulator websim Extensions",
        project_urls = {"Source code": "https://github.com/robotpy/mostrobotpy"},
        author_email = "RobotPy Development Team <robotpy@googlegroups.com>",
        requires = ["robotpy-native-wpihal==0.0.0", "robotpy-native-wpinet==0.0.0"],
        python_requires = ">=3.12",
        entry_points = {
            "robotpy_sim.2027": ["ws-server = halsim_ws.server", "ws-client = halsim_ws.client"],
        },
        visibility = ["//visibility:public"],
    )
