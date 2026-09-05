"""Helpers for optional Dear ImGui test-engine based tests."""

load("@rules_cc//cc:defs.bzl", "cc_test")

IMGUI_TEST_ENV = {
    "SDL_RENDER_DRIVER": "software",
    "SDL_VIDEO_DRIVER": "dummy",
    "SDL_VIDEO_DUMMY_SAVE_FRAMES": "0",
    "WPIGUI_FORCE_RENDERER": "2d",
}

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
    cc_test(
        name = name,
        srcs = srcs,
        defines = defines + ["RUNNING_IMGUI_TESTS"],
        deps = deps + ["//wpigui:wpigui_imgui_test"],
        env = IMGUI_TEST_ENV,
        tags = tags + [
            "imgui-test",
            "wpi-cpp-gui",
        ],
        target_compatible_with = imgui_test_target_compatible_with(),
        **kwargs
    )
