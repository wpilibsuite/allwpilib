load("@rules_python//python:defs.bzl", "py_test")

visibility([
    "//wpilibcExamples/...",
    "//wpilibjExamples/...",
])

_MAIN = Label("//shared:examplecheck.py")

def examplecheck_test(
        name,
        config,
        srcs,
        tags = [],
        package_complete = True,
        **kwargs):
    if "no-ide" not in tags:
        tags.append("no-ide")

    py_test(
        name = name,
        size = "small",
        srcs = [_MAIN],
        args = [
            # "$(location " + config + ")",
        ],
        data = [config] + srcs,
        main = _MAIN,
        tags = tags,
        **kwargs
    )

    bare_robots = native.glob(["**/Robot.*"], allow_empty = True)
    if package_complete and bare_robots:
        fail("Robots missing BUILD files:\n - " + "\n - ".join(bare_robots))
