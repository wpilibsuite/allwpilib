"""Helpers for optional Dear ImGui test-engine based tests."""

load("@rules_cc//cc:defs.bzl", "cc_binary")

IMGUI_TEST_ENGINE_DEFINES = [
    "IMGUI_ENABLE_TEST_ENGINE",
    "IMGUI_TEST_ENGINE_ENABLE_CAPTURE=0",
    "IMGUI_TEST_ENGINE_ENABLE_COROUTINE_STDTHREAD_IMPL=1",
]

IMGUI_TEST_ENV = {
    "SDL_RENDER_DRIVER": "software",
    "SDL_VIDEO_DRIVER": "dummy",
    "SDL_VIDEO_DUMMY_SAVE_FRAMES": "0",
    "WPIGUI_FORCE_RENDERER": "2d",
}

def _imgui_test_transition_impl(_, attr):
    return {
        "//shared/bazel/rules:with_imgui_test_engine_hooks": True,
    }

_imgui_test_transition = transition(
    implementation = _imgui_test_transition_impl,
    inputs = [],
    outputs = [
        "//shared/bazel/rules:with_imgui_test_engine_hooks",
    ],
)

def _imgui_cc_test_wrapper_impl(ctx):
    binary = ctx.executable.binary
    binary_target = ctx.attr.binary
    if type(binary_target) == "list":
        binary_target = binary_target[0]

    ctx.actions.symlink(
        output = ctx.outputs.executable,
        target_file = binary,
        is_executable = True,
    )

    return [DefaultInfo(
        executable = ctx.outputs.executable,
        files = depset([ctx.outputs.executable]),
        runfiles = ctx.runfiles(files = [binary]).merge(
            binary_target[DefaultInfo].default_runfiles,
        ),
    ), RunEnvironmentInfo(
        environment = ctx.attr.env,
        inherited_environment = ctx.attr.env_inherit,
    )]

_imgui_cc_test_wrapper_test = rule(
    implementation = _imgui_cc_test_wrapper_impl,
    attrs = {
        "binary": attr.label(
            executable = True,
            cfg = _imgui_test_transition,
            mandatory = True,
        ),
        "env": attr.string_dict(),
        "env_inherit": attr.string_list(),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
    },
    test = True,
)

def imgui_test_target_compatible_with():
    return select({
        "//shared/bazel/rules:with_imgui_tests_enabled": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }) + select({
        "@wpilib_toolchains//constraints/is_systemcore:systemcore": ["@platforms//:incompatible"],
        "//conditions:default": [],
    })

def wpilib_imgui_cc_test(
        name,
        srcs,
        deps = [],
        defines = [],
        tags = [],
        **kwargs):
    binary_name = name + "_binary"
    test_env = dict(IMGUI_TEST_ENV)
    test_env.update(kwargs.pop("env", {}))
    test_tags = tags + [
        "imgui-test",
        "wpi-cpp-gui",
    ]

    wrapper_kwargs = {
        "env": test_env,
        "env_inherit": kwargs.pop("env_inherit", []),
        "tags": test_tags,
        "target_compatible_with": imgui_test_target_compatible_with(),
        "testonly": True,
    }
    size = kwargs.pop("size", None)
    if size != None:
        wrapper_kwargs["size"] = size
    visibility = kwargs.pop("visibility", None)
    if visibility != None:
        wrapper_kwargs["visibility"] = visibility

    cc_binary(
        name = binary_name,
        testonly = True,
        srcs = srcs,
        defines = defines + ["RUNNING_IMGUI_TESTS"],
        deps = deps + ["//wpigui:wpigui_test_runner"],
        tags = test_tags + ["manual"],
        target_compatible_with = imgui_test_target_compatible_with(),
        **kwargs
    )

    _imgui_cc_test_wrapper_test(
        name = name,
        binary = ":" + binary_name,
        **wrapper_kwargs
    )
