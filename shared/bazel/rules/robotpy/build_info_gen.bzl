load("@bazel_lib//lib:write_source_files.bzl", "write_source_files")
load("//shared/bazel/rules/robotpy:compatibility_select.bzl", "robotpy_compatibility_select")

def generate_robotpy_native_wrapper_build_info(
        name,
        pyproject_toml,
        third_party_dirs = [],
        native_srcs_root = "src/main/native/",
        generated_include_target = None,
        generated_include_root = "src/generated/main/native/include"):
    """
    This function will generate the bazel file necessary to declare a library that wraps a standard allwpilib library.

    Params:
        pyproject_toml - Path to the native library wrappers definition file
        third_party_dirs - Any directories under <native_srcs_root>thirdparty that should be used by semiwrap
        native_srcs_root - Package-relative prefix under which the native cpp/include/thirdparty
            directories live, relative to wherever this macro is invoked from
        generated_include_target - Optional label pointing at a public filegroup exposing that project's
            generated headers
        generated_include_root - Package-relative prefix to strip from generated_include_target's sources
            so they land correctly under the wheel's include root. Defaults to the standard generated
            include directory; override when generated_include_target's sources live elsewhere (e.g.
            wpimath's generated protobuf headers live under src/generated/main/native/cpp).
    """
    cmd = "$(location //shared/bazel/rules/robotpy:generate_native_build_file) --output_file=$(OUTS)"
    cmd += " --project_cfg=$(location " + pyproject_toml + ")"
    if native_srcs_root:
        cmd += " --native_srcs_root=" + native_srcs_root
    cmd += " --package_name=" + native.package_name()
    if generated_include_target:
        cmd += " --generated_include_target=" + generated_include_target
        cmd += " --generated_include_root=" + generated_include_root
    if third_party_dirs:
        cmd += " --third_party_dirs "
        for d in third_party_dirs:
            cmd += " " + d
    native.genrule(
        name = "{}.gen_build_info".format(name),
        tools = ["//shared/bazel/rules/robotpy:generate_native_build_file"],
        srcs = [pyproject_toml],
        outs = ["{}-generated_build_info.bzl".format(name)],
        cmd = cmd,
        tags = ["robotpy"],
        target_compatible_with = robotpy_compatibility_select(),
    )

    write_source_files(
        name = "{}.generate_build_info".format(name),
        files = {
            "robotpy_native_build_info.bzl": "{}-generated_build_info.bzl".format(name),
        },
        visibility = ["//visibility:public"],
        suggested_update_target = "//:write_robotpy_generated_native_files",
        tags = ["robotpy"],
        target_compatible_with = robotpy_compatibility_select(),
        diff_args = ["-u"],
    )

def generate_robotpy_pybind_build_info(
        name,
        package_root_file,
        yaml_files = [],
        pkgcfgs = [],
        additional_srcs = [],
        generated_file_name = "robotpy_pybind_build_info.bzl",
        pyproject_toml = "src/main/python/pyproject.toml",
        stripped_include_prefix = None,
        yml_prefix = None):
    """
    This function will generate the bazel file necessary to build a pybind library with all of its extensions.

    Params:
        package_root_file - An __init__.py file used to key the semiwrap wrappers on the project root.
        yaml_files - All of the yaml files used by semi wrap to run library wrapping
        pkgcfgs - Local files used to trick semiwrap into thinking a library is installed
        additional_srcs - Any additional sources needed by the semiwrap process
        generated_file_name - Indicates the path of the auto-generated file
        pyproject_toml - Location of the pyproject.toml file that defines this project
        yml_prefix - Optional. Used in the event that the yml files are in a non-standard location
    """

    cmd = "$(location //shared/bazel/rules/robotpy:generate_pybind_build_file) --project_file=$(location " + pyproject_toml + ") --output_file=$(OUTS)"

    cmd += " --package_root_file=" + package_root_file
    if stripped_include_prefix:
        cmd += " --stripped_include_prefix=" + stripped_include_prefix
    if yml_prefix:
        cmd += " --yml_prefix=" + yml_prefix

    if pkgcfgs:
        cmd += " --pkgcfgs "
        for x in pkgcfgs:
            cmd += " $(locations " + x + ")"

    native.genrule(
        name = "{}.gen_build_info".format(name),
        tools = ["//shared/bazel/rules/robotpy:generate_pybind_build_file"],
        srcs = [pyproject_toml, package_root_file] + yaml_files + pkgcfgs + additional_srcs + ["//shared/bazel/rules/robotpy:jinja_templates"],
        outs = ["{}-generated_build_info.bzl".format(name)],
        cmd = cmd,
        tags = ["robotpy"],
        target_compatible_with = robotpy_compatibility_select(),
    )

    write_source_files(
        name = "{}.generate_build_info".format(name),
        files = {
            generated_file_name: "{}-generated_build_info.bzl".format(name),
        },
        suggested_update_target = "//:write_robotpy_generated_pybind_files",
        visibility = ["//visibility:public"],
        tags = ["robotpy"],
        target_compatible_with = robotpy_compatibility_select(),
        diff_args = ["-u"],
    )
