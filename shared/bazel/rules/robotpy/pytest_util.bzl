load("@rules_python_pytest//python_pytest:defs.bzl", "py_pytest_test")
load("//shared/bazel/rules/robotpy:compatibility_select.bzl", "robotpy_compatibility_select")

def robotpy_py_test(name, tests, extra_sources = [], **kwargs):
    """
    Helper function which splits a collection of tests into individually runnable test targets
    """
    for test_file in tests:
        py_pytest_test(
            name = test_file[:-3],
            size = "small",
            srcs = [test_file] + extra_sources,
            target_compatible_with = robotpy_compatibility_select(),
            tags = [
                "no-asan",
                "no-tsan",
                "robotpy",
            ],
            legacy_create_init = 0,
            **kwargs
        )
