load("@platforms//host:constraints.bzl", "HOST_CONSTRAINTS")

def robotpy_compatibility_select():
    return select({
        "@bazel_tools//src/conditions:windows": ["@platforms//:incompatible"],
        "@wpilib_toolchains//constraints/is_systemcore:systemcore": ["@platforms//:incompatible"],
        "//conditions:default": [],
    })

def robotpy_host_only_select(values):
    # @platforms//host is a platform, not a valid select() condition. Match
    # its OS/CPU constraints so equivalent target platforms also get stubs.
    return select({
        "//shared/bazel/rules/robotpy:host_platform": values,
        "//conditions:default": [],
    })

def robotpy_host_compatibility():
    return HOST_CONSTRAINTS + robotpy_compatibility_select()
