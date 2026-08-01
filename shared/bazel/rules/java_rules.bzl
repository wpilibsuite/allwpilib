load("@rules_java//java:defs.bzl", "java_binary", "java_library", "java_test")
load("@rules_shell//shell:sh_binary.bzl", "sh_binary")
load("@rules_shell//shell:sh_test.bzl", "sh_test")
load("//shared/bazel/rules:packaging.bzl", "zip_java_srcs")
load("//shared/bazel/rules:publishing.bzl", "wpilib_maven_export")
load("//shared/bazel/rules/gen:flatten_native_libs.bzl", "wpilib_flatten_native_libs")

# Applied to every generated "_java_impl"/wrapper target pair (both the
# wpilib_java_junit5_test and wpilib_java_binary machinery below), so
# sanitizer builds and //... wildcard expansion treat them uniformly.
_INNER_TAGS = ["allwpilib-build-java", "no-asan", "no-tsan", "no-ubsan"]

def _native_companion_label(label):
    if ":" not in label:
        # Shorthand label (e.g. "//wpiannotations") where the target name is
        # implicitly the last path segment.
        target = label.split("/")[-1]
        return label + ":" + target + ".native"
    return label + ".native"

def _has_native_companion(label):
    if not label.startswith(":"):
        # Cross-package reference: by repo convention every internal "-java"
        # target is built via wpilib_java_library, which always declares a
        # `.native` sibling.
        return True

    # Same-package reference: not every same-package target goes through
    # wpilib_java_library (e.g. plain java_binary/java_library targets like
    # the wpilibjExamples example/snippet/command/template binaries), so only
    # rely on the sibling if it was actually declared so far in this package.
    return native.existing_rule(label[1:] + ".native") != None

def _internal_native_companions(labels):
    """Maps a list of dep labels to their sibling `.native` filegroup labels.

    External (`@...`) labels are skipped: they either carry no native code, or
    (like the prebuilt OpenCV Java artifact) already attach their native
    library through a plain, non-self-referential attribute, so they
    propagate through ordinary dep merging without needing a `.native`
    sibling. Same-package labels without a declared `.native` sibling (e.g.
    plain java_binary/java_library targets) are skipped too: they carry no
    native code of their own, and whatever real native libraries they depend
    on are already reachable through the caller's other, wpilib_java_library-
    backed deps.
    """
    return [
        _native_companion_label(label)
        for label in labels
        if not label.startswith("@") and _has_native_companion(label)
    ]

def wpilib_java_library(
        name,
        maven_group_id,
        maven_artifact_name,
        tags = [],
        extra_source_pkgs = [],
        native_libs = [],
        deps = [],
        runtime_deps = [],
        **kwargs):
    tags = list(tags) if tags else []

    maven_coordinates = "{}:{}:$(WPILIB_VERSION)".format(maven_group_id, maven_artifact_name)
    tags.append("maven_coordinates=" + maven_coordinates)

    java_library(
        name = name,
        tags = tags,
        deps = deps,
        runtime_deps = runtime_deps,
        **kwargs
    )

    zip_java_srcs(name = name, extra_pkgs = extra_source_pkgs)

    wpilib_maven_export(
        name = "{}_publish".format(name),
        classifier_artifacts = {"sources": ":lib{}-sources.jar".format(name)},
        lib_name = name,
        maven_coordinates = maven_coordinates,
        visibility = ["//visibility:public"],
    )

    # A sibling target carrying this library's own native/JNI shared libraries
    # (if any) plus those of its dependencies, transitively. This must be a
    # separate target from `name`: a JNI library's own shared object is
    # compiled using headers generated from `name` itself (see
    # wpilib_jni_java_library/_jni_headers in jni_rules.bzl), so attaching it
    # directly to `name`'s own data/deps would create a dependency cycle.
    native.filegroup(
        name = name + ".native",
        testonly = kwargs.get("testonly"),
        visibility = kwargs.get("visibility"),
        srcs = native_libs + _internal_native_companions(deps) + _internal_native_companions(runtime_deps),
    )

def wpilib_java_junit5_test(
        name,
        deps = [],
        runtime_deps = [],
        args = [],
        tags = [],
        package = "org.wpilib",
        jvm_flags = [],
        data = [],
        size = None,
        **kwargs):
    """
    Convenience helper to make a junit5 test
    """
    junit_deps = [
        "@maven//:org_junit_jupiter_junit_jupiter_api",
        "@maven//:org_junit_jupiter_junit_jupiter_params",
        "@maven//:org_junit_jupiter_junit_jupiter_engine",
    ]

    junit_runtime_deps = [
        "@maven//:org_junit_platform_junit_platform_console",
    ]

    # Collects every transitively-linked native/JNI shared library into one
    # flat directory, so the JVM/OS loader can resolve them uniformly
    # regardless of which Bazel package they were built in. Two channels feed
    # this: each dependency's `.native` sibling filegroup (see
    # wpilib_java_library above, for in-repo JNI modules whose own shared
    # library can't be reached through the ordinary dep graph without a
    # cycle), and the ordinary deps themselves (for things like the prebuilt
    # OpenCV Java artifact, which carries its native libraries through a
    # plain, non-self-referential attribute and so is already reachable via
    # normal transitive runfiles merging).
    native_libs_name = name + ".native-libs"
    wpilib_flatten_native_libs(
        name = native_libs_name,
        deps = deps + runtime_deps + _internal_native_companions(deps) + _internal_native_companions(runtime_deps),
        testonly = True,
        tags = ["manual"],
    )

    java_impl_name = name + "_java_impl"

    # The java_test itself never sets -Djava.library.path: the sh_test
    # wrapper below resolves the native-libs directory from the runfiles
    # manifest at test run time and injects it via --wrapper_script_flag
    # (applied after the params-file jvm_flags, so it always wins).
    # Tagged manual so //... wildcard expansion skips it; the sh_test below is
    # the only entry point users interact with.
    java_test(
        name = java_impl_name,
        deps = deps + junit_deps,
        runtime_deps = runtime_deps + junit_runtime_deps,
        data = data + [":" + native_libs_name],
        jvm_flags = jvm_flags + [
            "-ea",
            "--enable-native-access=ALL-UNNAMED",
        ],
        args = args + ["--select-package", package],
        main_class = "org.junit.platform.console.ConsoleLauncher",
        use_testrunner = False,
        testonly = True,
        tags = tags + ["manual"] + _INNER_TAGS,
        **kwargs
    )

    # Primary test target (all platforms). The wrapper reads
    # RUNFILES_MANIFEST_FILE to obtain the absolute native-libs path and
    # injects it via --wrapper_script_flag before invoking the java_impl
    # binary, on every OS.  On Windows the wrapper additionally prepends the
    # native-libs directory to PATH so the DLL loader can resolve transitive
    # dependencies.
    sh_test(
        name = name,
        srcs = ["//shared/bazel/rules:java_executable_wrapper.sh"],
        args = args + ["--select-package", package],
        deps = ["@bazel_tools//tools/bash/runfiles"],
        env = {
            "JAVA_EXECUTABLE_RLOCATION": "_main/" + native.package_name() + "/" + java_impl_name,
            "NATIVE_LIBS_RLOCATION": "_main/" + native.package_name() + "/" + native_libs_name,
        },
        size = size or "small",
        data = [":" + java_impl_name, ":" + native_libs_name],
        testonly = True,
        visibility = kwargs.get("visibility"),
        tags = tags + _INNER_TAGS,
    )

def wpilib_java_binary(
        name,
        deps = [],
        runtime_deps = [],
        halsim_deps = [],
        args = [],
        jvm_flags = [],
        tags = [],
        data = [],
        smoke_test = True,
        smoke_test_timeout_seconds = 10,
        **kwargs):
    """
    Convenience helper to make a runnable java_binary that can find its
    transitive native/JNI shared libraries at runtime (via `bazel run` or by
    executing the built target directly), the same way wpilib_java_junit5_test
    already does for `bazel test`. See that macro's comments for the full
    rationale; this mirrors its native-lib flattening + wrapper-script
    machinery, minus the JUnit-specific bits.

    halsim_deps: HAL simulation extension targets (e.g.
        //simulation/halsim_ws_client:shared/halsim_ws_client) to auto-load at
        startup via HALSIM_EXTENSIONS. Unlike deps/runtime_deps, these never
        reach the java_binary's own Java classpath (a cc_shared_library
        provides no JavaInfo) - they only flow into the native-libs flattening
        below, which also records their basenames for the wrapper script to
        turn into HALSIM_EXTENSIONS at run time.

    smoke_test: if true (the default), also generate a "<name>-smoke-test"
        target that runs the program for smoke_test_timeout_seconds and
        passes if it's still running at the end - these are simulated Robot
        programs that run forever once started, so a program that exits
        early indicates a runtime-only bug the compiler couldn't catch (e.g.
        two DigitalInputs constructed on the same port).
    """
    native_libs_name = name + ".native-libs"
    native_libs_kwargs = {}
    if "testonly" in kwargs:
        native_libs_kwargs["testonly"] = kwargs["testonly"]
    wpilib_flatten_native_libs(
        name = native_libs_name,
        deps = deps + runtime_deps + _internal_native_companions(deps) + _internal_native_companions(runtime_deps),
        halsim_deps = halsim_deps,
        tags = ["manual"],
        **native_libs_kwargs
    )

    java_impl_name = name + "_java_impl"

    # Core java_binary: never bakes in -Djava.library.path itself. The
    # sh_binary wrapper below resolves the native-libs directory from the
    # runfiles manifest at run time and injects it via --wrapper_script_flag.
    # Tagged manual so //... wildcard expansion skips it; the sh_binary below
    # is the only entry point users interact with.
    java_binary(
        name = java_impl_name,
        deps = deps,
        runtime_deps = runtime_deps,
        data = data + [":" + native_libs_name],
        jvm_flags = jvm_flags + [
            "--add-opens",
            "java.base/jdk.internal.vm=ALL-UNNAMED",
            "--add-opens",
            "java.base/java.lang=ALL-UNNAMED",
        ],
        args = args,
        tags = tags + ["manual"] + _INNER_TAGS,
        **kwargs
    )

    wrapper_env = {
        "HALSIM_MANIFEST_RLOCATION": "_main/" + native.package_name() + "/" + native_libs_name + ".halsim-extensions.txt",
        "JAVA_EXECUTABLE_RLOCATION": "_main/" + native.package_name() + "/" + java_impl_name,
        "NATIVE_LIBS_RLOCATION": "_main/" + native.package_name() + "/" + native_libs_name,
    }

    # Primary runnable target. The wrapper reads RUNFILES_MANIFEST_FILE to
    # obtain the absolute native-libs path and injects it via
    # --wrapper_script_flag before invoking the java_impl binary, on every
    # OS. `args` is duplicated here (rather than relying solely on the inner
    # java_binary's own `args`) because that attribute is only consulted by
    # Bazel's own `run` invocation of the target actually being run, not
    # baked into the generated stub itself.
    sh_binary(
        name = name,
        srcs = ["//shared/bazel/rules:java_executable_wrapper.sh"],
        args = args,
        deps = ["@bazel_tools//tools/bash/runfiles"],
        env = wrapper_env,
        data = [":" + java_impl_name, ":" + native_libs_name],
        visibility = kwargs.get("visibility"),
        tags = tags + _INNER_TAGS,
    )

    if smoke_test:
        # Runs the same program as the sh_binary above, except the wrapper
        # backgrounds it and watches it instead of exec'ing: still alive
        # after smoke_test_timeout_seconds means it started up cleanly (this
        # is a simulated Robot program - it never exits on its own), an
        # early exit means a runtime-only startup bug.
        #
        # Deliberately drops HALSIM_MANIFEST_RLOCATION from the shared
        # wrapper_env: the smoke test only cares whether the program itself
        # starts up cleanly, and loading halsim_gui would pop open its
        # simulation GUI window on every test run for no benefit here.
        smoke_test_env = dict(wrapper_env)
        smoke_test_env.pop("HALSIM_MANIFEST_RLOCATION", None)
        smoke_test_env["SMOKE_TEST_TIMEOUT_SECONDS"] = str(smoke_test_timeout_seconds)
        sh_test(
            name = name + "-smoke-test",
            srcs = ["//shared/bazel/rules:java_executable_wrapper.sh"],
            args = args,
            deps = ["@bazel_tools//tools/bash/runfiles"],
            env = smoke_test_env,
            size = "small",
            data = [":" + java_impl_name, ":" + native_libs_name],
            testonly = True,
            visibility = kwargs.get("visibility"),
            tags = tags + _INNER_TAGS,
        )
