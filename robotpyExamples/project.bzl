load("@rules_python//python:defs.bzl", "py_binary", "py_library", "py_test")

visibility([
    "//developerRobot/...",
    "//robotpyExamples/...",
])

_WPILIB_WHEEL = "//wpilibc:robotpy-wpilib"
_CLI_MAIN = "//robotpyExamples:robotpy_entry_point"

def robotpy_project(
        name,
        srcs,
        main,
        data = [],
        deps = [],
        sim_deps = [
            "//simulation/halsim_gui:robotpy-halsim-gui",
        ],
        test_srcs = [],
        test_data = [],
        test_deps = [],
        size = "small",
        **kwargs):
    """
    Creates a RobotPy robot project.

    Targets:
      {name}-lib
      {name}
      {name}-sim
      {name}-test

    Args:
      main (string): the path to the file with the robot class
    """

    if _WPILIB_WHEEL not in deps:
        # Every robot project depends on WPILib core; the CLI looks for a RobotBase subclass.
        deps = deps + [_WPILIB_WHEEL]
    test_options = ["--builtin"] if not test_srcs else []

    py_library(
        name = name + "-lib",
        srcs = srcs,
        data = data,
        deps = deps,
        imports = ["."],
        **kwargs
    )

    # The above py_library will compile the main file.
    # Listing it as srcs to the binary targets will cause conflicts.
    executable_data = [main]
    main_option = ["--main", "$(location " + main + ")"]

    # For examples list check
    native.exports_files(
        srcs = [main],
        visibility = ["//robotpyExamples:example_kinds"],
    )

    executable_srcs = [_CLI_MAIN]
    executable_deps = [
        ":" + name + "-lib",
        "//robotpyExamples:robotpy",
    ]
    tags = kwargs.pop("tags", [])

    py_binary(
        name = name,
        srcs = executable_srcs,
        data = executable_data,
        main = _CLI_MAIN,
        args = main_option,
        deps = executable_deps,
        tags = tags + ["manual", "no-ide"],
        **kwargs
    )

    py_binary(
        name = name + "-sim",
        srcs = executable_srcs,
        data = executable_data,
        main = _CLI_MAIN,
        args = main_option + ["sim"],
        deps = executable_deps + sim_deps,
        tags = tags + ["manual", "no-ide"],
        **kwargs
    )

    py_test(
        name = name + "-test",
        srcs = test_srcs + executable_srcs,
        main = _CLI_MAIN,
        data = test_data + executable_data,
        args = main_option + ["test"] + test_options,
        deps = test_deps + executable_deps,
        size = size,
        tags = tags,
        **kwargs
    )
