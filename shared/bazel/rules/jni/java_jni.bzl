load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")
load("//shared/bazel/rules:java_rules.bzl", "wpilib_java_library")

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

def _merge_default_infos(ctx, infos):
    return DefaultInfo(
        files = depset(transitive = [info.files for info in infos]),
        runfiles = ctx.runfiles(
            transitive_files = depset(
                transitive = [info.default_runfiles.files for info in infos] + [info.data_runfiles.files for info in infos],
            ),
        ),
    )

def _merge_java_infos_impl(ctx):
    return [
        _merge_default_infos(ctx, [lib[DefaultInfo] for lib in ctx.attr.libs + ctx.attr.native_libs]),
        java_common.merge([lib[JavaInfo] for lib in ctx.attr.libs]),
        cc_common.merge_cc_infos(direct_cc_infos = [lib[CcInfo] for lib in ctx.attr.native_libs]),
        coverage_common.instrumented_files_info(
            ctx,
            dependency_attributes = ["libs"],
        ),
    ]

merge_java_infos = rule(
    implementation = _merge_java_infos_impl,
    attrs = {
        "libs": attr.label_list(
            providers = [JavaInfo],
        ),
        "native_libs": attr.label_list(
            providers = [CcInfo],
        ),
    },
    provides = [JavaInfo],
)

def wpilib_java_jni_library(
        name,
        native_libs = [],
        **java_library_args):
    tags = java_library_args.pop("tags", default = None)
    visibility = java_library_args.pop("visibility", default = None)
    testonly = java_library_args.pop("testonly", default = None)

    original_name = name + ".intermediate_java"
    headers_name = name + ".hdrs"

    wpilib_java_library(
        name = original_name,
        # tags = ["manual"],
        visibility = ["//visibility:private"],
        testonly = testonly,
        **java_library_args
    )

    jni = "//shared/bazel/toolchains/jni:jni"
    _jni_headers(
        name = headers_name,
        tags = ["manual"],
        jni = jni,
        lib = ":" + original_name,
        testonly = testonly,
        visibility = visibility,
    )

    merge_java_infos(
        name = name,
        libs = [":" + original_name],
        native_libs = [x + ".shared" for x in native_libs],
        tags = tags,
        testonly = testonly,
        visibility = visibility,
    )
