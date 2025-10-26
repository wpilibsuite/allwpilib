load("//shared/bazel/rules:cc_rules.bzl", "wpilib_cc_library", "wpilib_cc_shared_library", "wpilib_cc_static_library")
load("//shared/bazel/rules:packaging.bzl", "package_minimal_cc_project")
load("//shared/bazel/rules:publishing.bzl", "host_architectures")

def wpilib_halsim_extension(
        name,
        static_init_extension_name,
        deps = [],
        dynamic_deps = [],
        static_deps = [],
        shared_additional_linker_inputs = [],
        shared_user_link_flags = [],
        **kwargs):
    """
    Helper wrapper for creating a HALSIM extension. Builds the base, static, and shared libraries as well as sets up publishing

    This provides the following outputs:
    <name>        - Base library
    <name>_static - Used to help the static library creation. Uses the same compilation args as the base library, plus defining `HALSIM_InitExtension`
    shared/<name> - Shared library
    static/<name> - Static library
    <name>-cpp_publish.publish - Publishing target.

    Params:
    name: Name of the base library.
    deps: Dependencies, used in creating the base and static base library
    dynamic_deps: cc_shared_libraries used for linking the shared library
    static_deps: cc_static_libraries used for linking the static library
    linkopts: Linker arguments used for creating the base library
    shared_additional_linker_inputs - See cc_shared_library.additional_linker_inputs
    shared_user_link_flags - See cc_shared_library.user_link_flags
    kwargs: Remaining arguments forwarded to the base and static base libraries.
    """
    wpilib_cc_library(
        name = name,
        include_license_files = True,
        target_compatible_with = select({
            "@rules_bzlmodrio_toolchains//constraints/is_roborio:roborio": ["@platforms//:incompatible"],
            "@rules_bzlmodrio_toolchains//constraints/is_systemcore:systemcore": ["@platforms//:incompatible"],
            "//conditions:default": [],
        }),
        visibility = ["//visibility:public"],
        deps = deps,
        **kwargs
    )

    wpilib_cc_library(
        name = "{}_static".format(name),
        copts = [
            "-DHALSIM_InitExtension=" + static_init_extension_name,
        ],
        include_license_files = True,
        target_compatible_with = select({
            "@rules_bzlmodrio_toolchains//constraints/is_roborio:roborio": ["@platforms//:incompatible"],
            "@rules_bzlmodrio_toolchains//constraints/is_systemcore:systemcore": ["@platforms//:incompatible"],
            "//conditions:default": [],
        }),
        visibility = ["//visibility:public"],
        deps = deps,
        **kwargs
    )

    wpilib_cc_shared_library(
        name = "shared/{}".format(name),
        auto_export_windows_symbols = False,
        dynamic_deps = dynamic_deps,
        additional_linker_inputs = shared_additional_linker_inputs,
        user_link_flags = shared_user_link_flags,
        visibility = ["//visibility:public"],
        deps = [":{}".format(name)],
    )

    wpilib_cc_static_library(
        name = "static/{}".format(name),
        static_deps = static_deps,
        visibility = ["//visibility:public"],
        deps = [":{}_static".format(name)],
    )

    package_minimal_cc_project(
        name = name,
        architectures = host_architectures,
        maven_artifact_name = name,
        maven_group_id = "edu.wpi.first.halsim",
    )
