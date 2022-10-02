load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_import", "cc_library", "cc_test")

def wpilib_cc_library(
        name,
        tags = [],
        **kwargs):
    cc_library(
        name = name,
        tags = tags + ["allwpilib-build-cpp"],
        **kwargs
    )

def wpilib_cc_binary(
        name,
        tags = [],
        **kwargs):
    cc_binary(
        name = name,
        tags = tags + ["allwpilib-build-cpp"],
        **kwargs
    )

def wpilib_cc_test(
        name,
        tags = [],
        **kwargs):
    cc_test(
        name = name,
        tags = tags + ["allwpilib-build-cpp", "no-roborio", "no-bionic", "no-raspbian"],
        **kwargs
    )

def wpilib_cc_shared_library(
        name,
        srcs = [],
        deps = [],
        tags = [],
        visibility = None,
        **shared_binary_kwargs):
    # Make the visibility private, so it can only be available through the alias
    shared_lib_name = name
    wpilib_cc_binary(
        name = shared_lib_name,
        srcs = srcs,
        deps = deps,
        linkshared = 1,
        tags = tags,
        visibility = ["//visibility:private"],
        **shared_binary_kwargs
    )

    # Because we cannot directly depend on cc_binary from other cc rules in deps attribute,
    # we use cc_import as a bridge to depend on the dll.

    # Get the import library for the dll
    native.filegroup(
        name = name + ".shared_import",
        srcs = [":" + shared_lib_name],
        output_group = "interface_library",
        tags = tags,
        visibility = ["//visibility:private"],
    )
    shared_lib_import_name = name + ".import"
    cc_import(
        name = shared_lib_import_name,
        interface_library = select({
            "@bazel_tools//src/conditions:windows": ":" + name + ".shared_import",
            "//conditions:default": None,
        }),
        shared_library = ":" + shared_lib_name,
        visibility = ["//visibility:private"],
    )

    # Finally, create a library that can be depended on
    wpilib_cc_library(
        name = name + ".shared",
        deps = deps + [":" + shared_lib_import_name],
        tags = tags,
        visibility = visibility,
    )

# https://github.com/bazelbuild/bazel/blob/26c7e10739907332e70d31e68d2bd2ff2e9a84fb/examples/windows/dll
def wpilib_cc_static_and_shared_library(
        name,
        srcs = [],
        hdrs = [],
        includes = [],
        features = [],
        defines = [],
        standard_deps = [],
        wpi_maybe_shared_deps = [],
        visibility = None,
        strip_include_prefix = None,
        export_symbols = True):
    headers_name = name + ".headers"
    wpilib_cc_library(
        name = headers_name,
        hdrs = hdrs,
        strip_include_prefix = strip_include_prefix,
    )

    # Bundle the sources, so it appears as they are only used once
    native.filegroup(
        name = name + ".sources",
        srcs = srcs,
    )

    static_lib_name = name + ".static"
    static_deps = standard_deps + [x + ".static" for x in wpi_maybe_shared_deps]
    wpilib_cc_library(
        name = static_lib_name,
        srcs = [name + ".sources"],
        includes = includes,
        defines = defines,
        deps = static_deps + [":" + headers_name],
        visibility = visibility,
    )

    shared_deps = standard_deps + [x + ".shared" for x in wpi_maybe_shared_deps] + [":" + headers_name]
    shared_features = list(features)
    if export_symbols:
        shared_features.append("windows_export_all_symbols")

    wpilib_cc_shared_library(
        name = name,
        srcs = [name + ".sources"],
        deps = shared_deps,
        defines = defines,
        visibility = visibility,
        features = shared_features,
        includes = includes,
    )
