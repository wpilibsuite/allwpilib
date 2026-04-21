def robotpy_compatibility_select():
    return select({
        "@bazel_tools//src/conditions:windows": ["@platforms//:incompatible"],
        "@rules_bzlmodrio_toolchains//constraints/is_systemcore:systemcore": ["@platforms//:incompatible"],
        "//conditions:default": [],
    })
