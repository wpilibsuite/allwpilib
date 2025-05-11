load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_pkg//:mappings.bzl", "pkg_files")
load("@rules_pkg//:pkg.bzl", "pkg_zip")

def third_party_cc_lib_helper(
        name,
        include_root,
        src_root = None,
        src_excludes = [],
        visibility = None):
    cc_library(
        name = name + "-headers",
        hdrs = native.glob([
            include_root + "/**",
        ]),
        includes = [include_root],
        strip_include_prefix = include_root,
        visibility = visibility,
    )

    pkg_files(
        name = name + "-headers-pkg",
        srcs = native.glob([include_root + "/**"]),
        strip_prefix = include_root,
    )

    if src_root:
        native.filegroup(
            name = name + "-srcs",
            srcs = native.glob([src_root + "/**"], exclude = src_excludes),
            visibility = visibility,
        )

        pkg_files(
            name = name + "-srcs-pkg",
            srcs = native.glob([src_root + "/**"]),
            strip_prefix = src_root,
        )

def wpilib_cc_library(
        name,
        srcs = [],
        hdrs = [],
        deps = [],
        third_party_libraries = [],
        thrid_party_header_only_libraries = [],
        extra_src_pkg_files = [],
        extra_hdr_pkg_files = [],
        include_license_files = False,
        **kwargs):
    maybe_liscense_pkg = ["//:liscense_pkg_files"] if include_license_files else []

    cc_library(
        name = name,
        hdrs = hdrs,
        srcs = srcs + [lib + "-srcs" for lib in third_party_libraries],
        deps = deps + [lib + "-headers" for lib in third_party_libraries + thrid_party_header_only_libraries],
        **kwargs
    )

    pkg_files(
        name = name + "-srcs-pkg",
        srcs = native.glob(["src/main/native/cpp/**"]),
        strip_prefix = "src/main/native/cpp",
        tags = ["manual"],
    )

    pkg_zip(
        name = name + "-srcs-zip",
        srcs = maybe_liscense_pkg + extra_src_pkg_files + [name + "-srcs-pkg"] + [lib + "-srcs-pkg" for lib in third_party_libraries],
        tags = ["manual", "no-remote"],
    )

    pkg_files(
        name = name + "-headers-pkg",
        srcs = native.glob(["src/main/native/include/**"]),
        strip_prefix = "src/main/native/include",
        tags = ["manual"],
    )

    pkg_zip(
        name = name + "-hdrs-zip",
        srcs = extra_hdr_pkg_files + maybe_liscense_pkg + [name + "-headers-pkg"] + [lib + "-headers-pkg" for lib in third_party_libraries + thrid_party_header_only_libraries],
        tags = ["manual", "no-remote"],
    )

    pkg_files(
        name = name + ".pkg",
        srcs = [":" + name],
        tags = ["manual"],
        prefix = "linux/x86-64/static",  # TODO(pjreiniger) Make cross platform
    )

    pkg_zip(
        name = name + "-zip",
        srcs = ["//:liscense_pkg_files"] + [name + ".pkg"],
        tags = ["manual", "no-remote"],
    )
