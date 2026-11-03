load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "robotpy_library")

def define_pybind_library(name, pkgcfgs = [], extra_pybind_hdrs = []):
    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["commands2/**"], exclude = ["commands2/**/*.py"]),
        tags = ["manual", "robotpy"],
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-commands-v2",
        srcs = native.glob(["commands2/**/*.py"]) + [
        ],
        data = [
            "{}.extra_files".format(name),
        ],
        imports = ["."],
        deps = [
            "//wpilibc/src/main/python",
        ],
        strip_path_prefixes = ["commandsv2/src/main/python/"],
        summary = "WPILib command framework v2",
        project_urls = {"Source code": "https://github.com/robotpy/mostrobotpy"},
        author_email = "RobotPy Development Team <robotpy@googlegroups.com>",
        requires = ["wpilib==0.0.0"],
        python_requires = ">=3.12",
        description_file = "README.md",
        extra_distinfo_files = {"LICENSE": "licenses/LICENSE"},
        entry_points = {
        },
        visibility = ["//visibility:public"],
    )
