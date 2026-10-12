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
        example_file_glob = "**/Robot.*",
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

    # If specified, require all matching files to be in subpackages.
    if example_file_glob:
        unpackaged_srcs = native.glob([example_file_glob], allow_empty = True)
        if unpackaged_srcs:
            fail("Examples missing BUILD files:\n - " + "\n - ".join(unpackaged_srcs))
