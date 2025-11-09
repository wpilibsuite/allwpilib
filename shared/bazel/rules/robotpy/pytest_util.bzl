load("@rules_python_pytest//python_pytest:defs.bzl", "py_pytest_test")
load("//shared/bazel/rules/robotpy:compatibility_select.bzl", "robotpy_compatibility_select")

def robotpy_py_test(name, srcs, **kwargs):
    py_pytest_test(
        name = name,
        size = "small",
        srcs = srcs,
        target_compatible_with = robotpy_compatibility_select(),
        tags = [
            "no-asan",
            "no-tsan",
            "robotpy",
        ],
        legacy_create_init = 0,
        **kwargs
    )
