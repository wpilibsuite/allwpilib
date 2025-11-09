load("@allwpilib_pip_deps//:requirements.bzl", "requirement")
load("@aspect_bazel_lib//lib:write_source_files.bzl", "write_source_files")
load("@rules_python//python:defs.bzl", "py_test")
load("//shared/bazel/rules/robotpy:compatibility_select.bzl", "robotpy_compatibility_select")

def __update_yaml_files_impl(ctx):
    output_dir = ctx.actions.declare_directory(ctx.attr.gen_dir)

    args = ctx.actions.args()
    args.add("semiwrap.tool")
    args.add("update-yaml")
    args.add("--write")
    args.add("-v")
    args.add("--project_file=" + ctx.files.pyproject_toml[0].path)
    args.add("--override_output_directory=" + output_dir.path)

    if ctx.files.pkgcfgs:
        args.add("--pkgcfgs")
        for f in ctx.files.pkgcfgs:
            args.add(str(f.path))

    ctx.actions.run(
        inputs = ctx.files.package_root_file + ctx.files.pyproject_toml + ctx.files.pkgcfgs + ctx.files.extra_hdrs + ctx.files.yaml_files,
        outputs = [output_dir],
        executable = ctx.executable._tool,
        arguments = [args],
    )

    return [DefaultInfo(files = depset([output_dir]))]

__update_yaml_files = rule(
    implementation = __update_yaml_files_impl,
    attrs = {
        "extra_hdrs": attr.label_list(allow_files = True),
        "gen_dir": attr.string(mandatory = True),
        "package_root_file": attr.label(mandatory = True, allow_files = True),
        "pkgcfgs": attr.label_list(allow_files = True),
        "pyproject_toml": attr.label(mandatory = True, allow_files = True),
        "yaml_files": attr.label_list(mandatory = True, allow_files = True),
        "_tool": attr.label(
            default = Label("//shared/bazel/rules/robotpy:wrapper"),
            cfg = "exec",
            executable = True,
        ),
    },
)

def update_yaml_files(name, yaml_output_directory = "src/main/python/semiwrap", **kwargs):
    __update_yaml_files(
        name = name,
        gen_dir = "{}_gen_update_yaml".format(name),
        target_compatible_with = robotpy_compatibility_select(),
        **kwargs
    )

    write_source_files(
        name = "write_{}".format(name),
        files = {
            yaml_output_directory: ":" + name,
        },
        suggested_update_target = "//:write_all",
        target_compatible_with = robotpy_compatibility_select(),
        visibility = ["//visibility:public"],
    )

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
