load("@rules_java//java:defs.bzl", "java_binary", "java_library", "java_test")

def _maybe_add_library(shared_lib_native_deps, library):
    if library.dynamic_library and not library.static_library:
        shared_lib_native_deps.append(library.dynamic_library)

def _get_dynamic_deps(target):
    shared_lib_native_deps = []

    if CcInfo in target:
        for linker_input in target[CcInfo].linking_context.linker_inputs.to_list():
            for library in linker_input.libraries:
                _maybe_add_library(shared_lib_native_deps, library)
    if JavaInfo in target:
        for library in target[JavaInfo].transitive_native_libraries.to_list():
            _maybe_add_library(shared_lib_native_deps, library)

    return shared_lib_native_deps

def _symlink_java_native_libraries_impl(ctx):
    shared_lib_native_deps = []
    for dep in ctx.attr.deps:
        for d in _get_dynamic_deps(dep):
            if d not in shared_lib_native_deps:
                shared_lib_native_deps.append(d)

    symlinks = []
    for lib in shared_lib_native_deps:
        out = ctx.actions.declare_file(ctx.attr.output_directory + "/" + lib.basename)
        if out not in symlinks:
            ctx.actions.symlink(output = out, target_file = lib)
            symlinks.append(out)

    return [DefaultInfo(files = depset(symlinks), runfiles = ctx.runfiles(files = symlinks))]

_symlink_java_native_libraries = rule(
    attrs = {
        "deps": attr.label_list(mandatory = True),
        "output_directory": attr.string(mandatory = True),
    },
    implementation = _symlink_java_native_libraries_impl,
)

def wpilib_java_library(
        name,
        tags = [],
        **kwargs):
    java_library(
        name = name,
        tags = tags + ["allwpilib-build-java"],
        **kwargs
    )

def wpilib_java_binary(name, deps = [], runtime_deps = [], data = [], tags = [], **kwargs):
    native_shared_libraries_symlink = name + ".symlink_native"
    _symlink_java_native_libraries(
        name = native_shared_libraries_symlink,
        deps = deps + runtime_deps,
        output_directory = select({
            "@bazel_tools//src/conditions:windows": name + ".exe.runfiles",
            "//conditions:default": name + ".runfiles",
        }),
        tags = ["manual"],
    )

    java_binary(
        name = name,
        data = data + [native_shared_libraries_symlink],
        jvm_flags = ["-Djava.library.path=."],
        deps = deps,
        tags = tags + ["allwpilib-build-java"],
        runtime_deps = runtime_deps,
        **kwargs
    )

def wpilib_java_junit5_test(
        name,
        deps = [],
        data = [],
        runtime_deps = [],
        args = [],
        tags = [],
        package = "edu",
        **kwargs):
    native_shared_libraries_symlink = name + ".symlink_native"
    extracted_native_dir = "extracted_native"
    full_extracted_native_dir = native.package_name() + "/extracted_native"
    _symlink_java_native_libraries(
        name = native_shared_libraries_symlink,
        deps = deps + runtime_deps,
        output_directory = select({
            "@bazel_tools//src/conditions:windows": name + ".exe.runfiles/__main__",
            "//conditions:default": extracted_native_dir,
        }),
        tags = ["manual"],
        testonly = True,
    )

    junit_deps = [
        "@maven//:org_junit_jupiter_junit_jupiter_api",
        "@maven//:org_junit_jupiter_junit_jupiter_params",
        "@maven//:org_junit_jupiter_junit_jupiter_engine",
    ]

    junit_runtime_deps = [
        "@maven//:org_junit_platform_junit_platform_commons",
        "@maven//:org_junit_platform_junit_platform_console",
        "@maven//:org_junit_platform_junit_platform_engine",
        "@maven//:org_junit_platform_junit_platform_launcher",
        "@maven//:org_junit_platform_junit_platform_suite_api",
    ]

    java_test(
        name = name,
        deps = deps + junit_deps,
        runtime_deps = runtime_deps + junit_runtime_deps,
        args = args + ["--select-package", package],
        main_class = "org.junit.platform.console.ConsoleLauncher",
        data = data + [native_shared_libraries_symlink],
        jvm_flags = select({
            "@bazel_tools//src/conditions:darwin": ["-Djava.library.path=" + full_extracted_native_dir],
            "@bazel_tools//src/conditions:linux_x86_64": ["-Djava.library.path=" + full_extracted_native_dir],
            "@bazel_tools//src/conditions:windows": ["-Djava.library.path=."],
        }),
        env = select({
            "@bazel_tools//src/conditions:darwin": {"LD_LIBRARY_PATH": full_extracted_native_dir},
            "@bazel_tools//src/conditions:linux_x86_64": {},
            "@bazel_tools//src/conditions:windows": {},
        }),
        use_testrunner = False,
        tags = tags + ["no-roborio", "no-bionic", "no-raspbian", "allwpilib-build-java", "no-asan", "no-tsan", "no-ubsan"],
        **kwargs
    )
