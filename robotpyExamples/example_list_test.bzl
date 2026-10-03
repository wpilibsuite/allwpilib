load("@rules_python//python:defs.bzl", "py_test")

_MAIN = "//robotpyExamples:check_example_list.py"

def example_list_test(
        name,
        config,
        robot_packages,
        size = "small",
        **kwargs):
    robots = [pkg + ":robot.py" for pkg in robot_packages]
    bare_robots = native.glob(["*/robot.py"], allow_empty = True)

    py_test(
        name = name,
        size = size,
        srcs = [_MAIN],
        args = [
            "$(location " + config + ")",
        ],
        data = [config] + robots,
        main = _MAIN,
        **kwargs
    )

    if bare_robots:
        fail("Robots missing BUILD files:\n - " + "\n - ".join(bare_robots))
