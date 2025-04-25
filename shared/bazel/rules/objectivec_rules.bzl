load("@rules_cc//cc:defs.bzl", "objc_library")

OBJC_COMPILER_FLAGS = [
    "-stdlib=libc++",
    "-fPIC",
]

def wpilib_objc_library(
        name,
        srcs = [],
        deps = [],
        copts = None,
        is_cpp = True,
        include_arc = True,
        **kwargs):
    copts = copts or []
    if is_cpp:
        copts.append("-std=c++20")
    if include_arc:
        copts += ["-fobjc-weak", "-fobjc-arc"]

    objc_library(
        name = name,
        srcs = srcs,
        copts = copts + OBJC_COMPILER_FLAGS,
        tags = ["manual"],  # This makes it so the other platforms will still build OK
        deps = deps,
        **kwargs
    )
