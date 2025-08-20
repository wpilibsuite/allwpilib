load("@allwpilib_pip_deps//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_test")
load("//shared/bazel/rules/robotpy:compatibility_select.bzl", "robotpy_compatibility_select")

def scan_headers(name, pyproject_toml, package_root_file, extra_hdrs, pkgcfgs):
    if pkgcfgs:
        pkgcfg_args = ["--pkgcfgs"]
        for pkgcfg in pkgcfgs:
            pkgcfg_args.append(" $(locations " + pkgcfg + ")")
    else:
        pkgcfg_args = []

    py_test(
        name = name,
        srcs = [
            "//shared/bazel/rules/robotpy:wrapper.py",
        ],
        deps = [
            "//shared/bazel/rules/robotpy:hack_pkgcfgs",
            requirement("semiwrap"),
        ],
        args = [
            "semiwrap.tool",
            "scan-headers",
            "--pyproject=$(location " + pyproject_toml + ")",
        ] + pkgcfg_args,
        data = extra_hdrs + pkgcfgs + [pyproject_toml, package_root_file],
        main = "shared/bazel/rules/robotpy/wrapper.py",
        size = "small",
        target_compatible_with = robotpy_compatibility_select(),
    )
