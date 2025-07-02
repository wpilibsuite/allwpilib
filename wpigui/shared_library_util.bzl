"""
This provides helper functions to build shared libraries that depend on wpigui.

Bazel does not pull transative objc libraries in for cc_shared_libraries, so you must add these functions to get them to link properly

https://github.com/bazelbuild/bazel/issues/21893
"""

def get_wpigui_additional_linker_inputs():
    return select({
        "@bazel_tools//src/conditions:darwin": [
            "//thirdparty/imgui_suite:glfw_src_darwin",
            "//thirdparty/imgui_suite:imgui_src_darwin",
            "//wpigui:wpigui-mac",
        ],
        "//conditions:default": [],
    })

def get_wpigui_user_link_flags():
    return select({
        "@bazel_tools//src/conditions:darwin": [
            "-Wl,-force_load,$(location //thirdparty/imgui_suite:glfw_src_darwin)",
            "-Wl,-force_load,$(location //thirdparty/imgui_suite:imgui_src_darwin)",
            "-Wl,-force_load,$(location //wpigui:wpigui-mac)",
            "-framework",
            "Metal",
            "-framework",
            "MetalKit",
            "-framework",
            "Cocoa",
            "-framework",
            "IOKit",
            "-framework",
            "CoreFoundation",
            "-framework",
            "CoreVideo",
            "-framework",
            "QuartzCore",
        ],
        "@bazel_tools//src/conditions:windows": [
            "-DEFAULTLIB:user32.lib",
            "-DEFAULTLIB:gdi32.lib",
            "-DEFAULTLIB:shell32.lib",
        ],
        "//conditions:default": [],
    })
