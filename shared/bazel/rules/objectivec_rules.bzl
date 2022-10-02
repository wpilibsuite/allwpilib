load("@rules_cc//cc:defs.bzl", "objc_library")

OBJC_COMPILER_FLAGS = [
    "-std=c++17",
    "-stdlib=libc++",
    "-fobjc-weak",
    "-fobjc-arc",
    "-fPIC",
]

def wpilib_objc_library(
        name,
        srcs = [],
        deps = [],
        copts = [],
        **kwargs):
    objc_library(
        name = name,
        srcs = srcs,
        copts = copts + OBJC_COMPILER_FLAGS,
        tags = ["manual"],  # This makes it so the other platforms will still build OK
        deps = deps,
        **kwargs
    )
