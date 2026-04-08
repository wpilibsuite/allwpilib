load("//shared/bazel/rules:cc_rules.bzl", "wpilib_cc_library")

def wpilib_halsim_extension(
        name,
        **kwargs):
    """
    Helper wrapper for creating a HALSIM extension. Provides some of the default argments for creating the library.
    """
    wpilib_cc_library(
        name = name,
        includes = ["src/main/native/include"],
        include_license_files = True,
        target_compatible_with = select({
            "@rules_bzlmodrio_toolchains//constraints/is_systemcore:systemcore": ["@platforms//:incompatible"],
            "//conditions:default": [],
        }),
        visibility = ["//visibility:public"],
        **kwargs
    )
