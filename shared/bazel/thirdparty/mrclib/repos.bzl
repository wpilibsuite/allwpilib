load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

cc_library_headers = """cc_library(
    name = "headers",
    hdrs = glob(["**"]),
    includes = ["."],
    visibility = ["//visibility:public"],
)
"""

cc_shared_library_build_contents = """
filegroup(
    name = "shared_interface",
    srcs = glob(["**/*.lib"], allow_empty=True),
    visibility = ["//visibility:public"],
)

filegroup(
    name = "shared_libs",
    srcs = glob([
            "**/*.dll",
            "**/*.so*",
            "**/*.dylib",
        ],
        allow_empty = True,
    ),
    visibility = ["//visibility:public"],
)
"""

def _mrclib_extension_impl(mctx):
    mrclib_maven_base = "https://frcmaven.wpi.edu/artifactory/development-2027"
    mrclib_version = "2027.1.0-alpha-1-42-g019903f"

    mrclib_hdr_hash = "sha256-xxAR4sWTdJrKWF7sMI85TV5/NEUcGhTN2OB36jsTaLU="
    mrclib_linuxarm64_hash = "sha256-Zmt+0UV0SVtL8rwvnV4TClwXA45WIMM/+0pWtUR/g+g="
    mrclib_linuxx86_64_hash = "sha256-DkLWVl94dNv9AU6iAI7t2ynODh13aStBWPLcOi7feZc="
    mrclib_osx_hash = "sha256-kdYgf+roEUQ0Jmb192dmM7VMm9g6oiltBfkKRRsQi7I="
    mrclib_systemcore_hash = "sha256-wcrUn7lsqnP928hSWIxnohEEAlObOdukAlXCGXQRToQ="
    mrclib_windowsarm64_hash = "sha256-TQVFNxOppDqMCgHSPFKL4eT7U+OXtXPs3YwczLJ0kcs="
    mrclib_windowsx86_64_hash = "sha256-s26W4tUjgg55D1utgkrdYNycB9XdcKlokKG0dkQJDSA="

    maybe(
        http_archive,
        "mrclib_headers",
        url = mrclib_maven_base + "/org/wpilib/mrclib/mrclib-cpp/{version}/mrclib-cpp-{version}-headers.zip".format(version = mrclib_version),
        integrity = mrclib_hdr_hash,
        build_file_content = cc_library_headers,
    )

    maybe(
        http_archive,
        "mrclib_linuxarm64",
        url = mrclib_maven_base + "/org/wpilib/mrclib/mrclib-cpp/{version}/mrclib-cpp-{version}-linuxarm64.zip".format(version = mrclib_version),
        integrity = mrclib_linuxarm64_hash,
        build_file_content = cc_shared_library_build_contents,
    )

    maybe(
        http_archive,
        "mrclib_linuxx86-64",
        url = mrclib_maven_base + "/org/wpilib/mrclib/mrclib-cpp/{version}/mrclib-cpp-{version}-linuxx86-64.zip".format(version = mrclib_version),
        integrity = mrclib_linuxx86_64_hash,
        build_file_content = cc_shared_library_build_contents,
    )

    maybe(
        http_archive,
        "mrclib_osx",
        url = mrclib_maven_base + "/org/wpilib/mrclib/mrclib-cpp/{version}/mrclib-cpp-{version}-osxuniversal.zip".format(version = mrclib_version),
        integrity = mrclib_osx_hash,
        build_file_content = cc_shared_library_build_contents,
    )

    maybe(
        http_archive,
        "mrclib_systemcore",
        url = mrclib_maven_base + "/org/wpilib/mrclib/mrclib-cpp/{version}/mrclib-cpp-{version}-linuxsystemcore.zip".format(version = mrclib_version),
        integrity = mrclib_systemcore_hash,
        build_file_content = cc_shared_library_build_contents,
    )

    maybe(
        http_archive,
        "mrclib_windowsarm64",
        url = mrclib_maven_base + "/org/wpilib/mrclib/mrclib-cpp/{version}/mrclib-cpp-{version}-windowsarm64.zip".format(version = mrclib_version),
        integrity = mrclib_windowsarm64_hash,
        build_file_content = cc_shared_library_build_contents,
    )

    maybe(
        http_archive,
        "mrclib_windowsx86-64",
        url = mrclib_maven_base + "/org/wpilib/mrclib/mrclib-cpp/{version}/mrclib-cpp-{version}-windowsx86-64.zip".format(version = mrclib_version),
        integrity = mrclib_windowsx86_64_hash,
        build_file_content = cc_shared_library_build_contents,
    )

mrclib_extension = module_extension(implementation = _mrclib_extension_impl)
