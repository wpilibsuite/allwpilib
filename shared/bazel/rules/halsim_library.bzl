load("//shared/bazel/rules:cc_rules.bzl", "wpilib_cc_library", "wpilib_cc_shared_library", "wpilib_cc_static_library")

def wpilib_halsim_extension(
        name,
        init_extension_name,
        srcs = [],
        deps = [],
        dynamic_deps = [],
        static_deps = [],
        auto_export_windows_symbols = True,
        visibility = None,
        shared_library_additional_linker_inputs = [],
        shared_library_user_link_flags = [],
        **kwargs):
    """
    Helper wrapper for creating a HALSIM extension. Provides some of the default argments for creating the library.
    """
    wpilib_cc_library(
        name = name,
        srcs = srcs,
        include_third_party_notices = True,
        target_compatible_with = select({
            "@rules_bzlmodrio_toolchains//constraints/is_roborio:roborio": ["@platforms//:incompatible"],
            "@rules_bzlmodrio_toolchains//constraints/is_systemcore:systemcore": ["@platforms//:incompatible"],
            "//conditions:default": [],
        }),
        visibility = visibility,
        deps = deps,
        **kwargs
    )

    wpilib_cc_library(
        name = "{}_static".format(name),
        srcs = srcs,
        copts = [
            "-DHALSIM_InitExtension=" + init_extension_name,
        ],
        include_third_party_notices = True,
        target_compatible_with = select({
            "@rules_bzlmodrio_toolchains//constraints/is_roborio:roborio": ["@platforms//:incompatible"],
            "@rules_bzlmodrio_toolchains//constraints/is_systemcore:systemcore": ["@platforms//:incompatible"],
            "//conditions:default": [],
        }),
        visibility = visibility,
        deps = deps,
        **kwargs
    )

    wpilib_cc_shared_library(
        name = "shared/{}".format(name),
        auto_export_windows_symbols = auto_export_windows_symbols,
        dynamic_deps = dynamic_deps,
        visibility = visibility,
        deps = [":{}".format(name)],
        additional_linker_inputs = shared_library_additional_linker_inputs,
        user_link_flags = shared_library_user_link_flags,
    )

    wpilib_cc_static_library(
        name = "static/{}".format(name),
        static_deps = static_deps,
        deps = [":{}".format(name)],
        visibility = visibility,
    )
