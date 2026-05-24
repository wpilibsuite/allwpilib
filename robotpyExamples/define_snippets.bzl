load("@allwpilib_pip_deps//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary", "py_test")
load("//robotpyExamples:snippet_projects.bzl", "SNIPPETS")

_SNIPPETS_BASE = "snippets"

def define_snippets():
    for snippet_name in SNIPPETS:
        snippet_folder = _SNIPPETS_BASE + "/" + snippet_name
        base_name = "snippet-" + snippet_name
        common_kwargs = dict(
            srcs = [":robotpy_entry_point.py"],
            main = "robotpy_entry_point.py",
            data = native.glob([snippet_folder + "/**"]),
            imports = [snippet_folder],
        )
        common_deps = [
            ":robotpy",
            "//apriltag:robotpy-apriltag",
            "//commandsv2:commandsv2-py",
            "//wpilibc:robotpy-wpilib",
            "//romiVendordep:robotpy-romi",
            "//xrpVendordep:robotpy-xrp",
            requirement("numpy"),
        ]

        py_test(
            name = base_name + "-test",
            args = ["--main", "$(location " + snippet_folder + "/robot.py)", "test", "--builtin"],
            deps = common_deps,
            size = "small",
            **common_kwargs
        )

        py_binary(
            name = base_name + "-run",
            args = ["--main", "$(location " + snippet_folder + "/robot.py)", "run"],
            deps = common_deps,
            **common_kwargs
        )

        py_binary(
            name = base_name + "-sim",
            args = ["--main", "$(location " + snippet_folder + "/robot.py)", "sim"],
            deps = common_deps + ["//simulation/halsim_gui:robotpy-halsim-gui"],
            **common_kwargs
        )
