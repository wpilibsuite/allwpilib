load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_java//java:defs.bzl", "java_library")
load("@rules_pkg//:mappings.bzl", "filter_directory")
load("//shared/bazel/rules:cc_rules.bzl", "wpilib_cc_shared_library", "wpilib_cc_static_library")

def _jni_headers_impl(ctx):
    include_dir = ctx.actions.declare_directory(ctx.attr.name + ".h")
    native_headers_jar = ctx.attr.lib[JavaInfo].outputs.native_headers
    args = ["xf", native_headers_jar.path, "-d", include_dir.path]

    ctx.actions.run(
        inputs = [native_headers_jar],
        tools = [ctx.executable._zipper],
        outputs = [include_dir],
        executable = ctx.executable._zipper.path,
        arguments = args,
    )

    cc_toolchain = find_cpp_toolchain(ctx)
    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = cc_toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )
    compilation_context, _ = cc_common.compile(
        name = ctx.attr.name,
        actions = ctx.actions,
        feature_configuration = feature_configuration,
        cc_toolchain = cc_toolchain,
        public_hdrs = [include_dir],
        quote_includes = [include_dir.path],
    )
    cc_info_with_jni = cc_common.merge_cc_infos(
        direct_cc_infos = [
            CcInfo(compilation_context = compilation_context),
            ctx.attr.jni[CcInfo],
        ],
    )

    return [
        DefaultInfo(files = depset([include_dir])),
        cc_info_with_jni,
    ]

_jni_headers = rule(
    implementation = _jni_headers_impl,
    attrs = {
        "jni": attr.label(mandatory = True),
        "lib": attr.label(
            mandatory = True,
            providers = [JavaInfo],
        ),
        "_cc_toolchain": attr.label(default = Label("@bazel_tools//tools/cpp:current_cc_toolchain")),
        "_zipper": attr.label(
            executable = True,
            cfg = "exec",
            default = Label("@bazel_tools//tools/zip:zipper"),
        ),
    },
    fragments = ["cpp"],
    incompatible_use_toolchain_transition = True,
    provides = [CcInfo],
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
)

def wpilib_jni_java_library(
        name,
        native_libs = [],
        **java_library_args):
    tags = java_library_args.pop("tags", default = None)
    visibility = java_library_args.pop("visibility", default = None)
    testonly = java_library_args.pop("testonly", default = None)
    headers_name = name + ".hdrs"
    java_library(
        name = name,
        visibility = visibility,
        testonly = testonly,
        tags = tags,
        **java_library_args
    )

    jni = "@rules_bzlmodrio_toolchains//jni"
    _jni_headers(
        name = headers_name,
        jni = jni,
        lib = ":" + name,
        testonly = testonly,
        visibility = visibility,
    )

    # Expose a pkg_files with the JNI generated header in it.
    filter_directory(
        name = name + "-jni-hdrs-pkg",
        src = headers_name,
        excludes = ["MANIFEST.MF"],
        outdir_name = "jni/",
    )

def wpilib_jni_cc_library(
        name,
        deps = [],
        dynamic_deps = [],
        static_deps = [],
        java_dep = None,
        **kwargs):
    """
    Helper macro to create a JNI library, as well as some additional artifacts used
    for publishing.

    Produces the following outputs:
        1. <name>        - standard cc_library
        2. static/<name> - cc_static_library with transative object symbols stripped out
        3. shared/<name> - cc_shared_library with transative object symbols stripped out

    Params
        deps: C++ Deps for the standard library
        dynamic_deps: Dynamic deps, used in the creation of the cc_shared_library only
        static_deps: Static deps, used in the creation of the cc_static_library only
        java_dep: The java library that contains the native interface definition. Used to extract
                  auto-generated jni header files.

    """

    jni = "@rules_bzlmodrio_toolchains//jni"

    if java_dep[0] != ":":
        fail("java_dep", java_dep, "should start with a :")

    cc_library(
        name = name,
        deps = [jni, java_dep + ".hdrs"] + deps,
        **kwargs
    )

    wpilib_cc_shared_library(
        name = "shared/{}".format(name),
        auto_export_windows_symbols = False,
        dynamic_deps = dynamic_deps,
        visibility = ["//visibility:public"],
        deps = [name],
    )

    wpilib_cc_static_library(
        name = "static/{}".format(name),
        static_deps = static_deps,
        visibility = ["//visibility:public"],
        deps = [name],
    )
