load("//shared/bazel/rules:cc_rules.bzl", "wpilib_cc_shared_library")

def wpilib_jni_cc_library(
        name,
        deps = [],
        standard_deps = [],
        wpi_maybe_shared_deps = [],
        java_dep = None,
        **kwargs):
    deps = standard_deps + [x + ".shared" for x in wpi_maybe_shared_deps]

    jni = "//shared/bazel/toolchains/jni:jni"
    wpilib_cc_shared_library(
        name = name,
        deps = [jni, java_dep + ".hdrs"] + deps,
        **kwargs
    )
