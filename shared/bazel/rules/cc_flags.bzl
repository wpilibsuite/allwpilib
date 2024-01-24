def get_wpigui_linker_flags(console):
    WINDOWS_FLAGS = [
        "-DEFAULTLIB:Gdi32.lib",
        "-DEFAULTLIB:Shell32.lib",
        "-DEFAULTLIB:d3d11.lib",
        "-DEFAULTLIB:d3dcompiler.lib",
    ]

    DARWIN_FLAGS = [
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
    ]

    if not console:
        WINDOWS_FLAGS.append("-SUBSYSTEM:WINDOWS")

    return select({
        "@bazel_tools//src/conditions:darwin": DARWIN_FLAGS,
        "@bazel_tools//src/conditions:linux_x86_64": ["-lX11"],
        "@bazel_tools//src/conditions:windows": WINDOWS_FLAGS,
        "@rules_bzlmodrio_toolchains//constraints/combined:is_cross_compiler": [],
    })

def get_libssh_linker_flags():
    WINDOWS_FLAGS = [
        "-DEFAULTLIB:ws2_32.lib",
        "-DEFAULTLIB:advapi32.lib",
        "-DEFAULTLIB:crypt32.lib",
        "-DEFAULTLIB:user32.lib",
    ]

    return select({
        "@bazel_tools//src/conditions:darwin": ["-framework", "Kerberos"],
        "@bazel_tools//src/conditions:linux_x86_64": [],
        "@bazel_tools//src/conditions:windows": WINDOWS_FLAGS,
        "@rules_bzlmodrio_toolchains//constraints/combined:is_cross_compiler": [],
    })
