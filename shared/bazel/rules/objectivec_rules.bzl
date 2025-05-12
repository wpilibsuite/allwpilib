load("@rules_cc//cc:defs.bzl", "objc_library")

# These flags are added to match the current native-utils flags in https://github.com/wpilibsuite/native-utils/blob/d8555ca479438c9c08b4a2400a58f280fec90c3f/src/main/java/edu/wpi/first/nativeutils/WPINativeUtilsExtension.java
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
    """
    Helper macro for defining .

    Args:
        is_cpp: If this is a C++ library. Specifying this will add additional C++ specific compiler flags
        include_arc: If true, this will add compiler flags for ARC (Automatic Reference Counting)
    """
    copts = copts or []
    if is_cpp:
        copts.append("-std=c++20")
    if include_arc:
        copts += ["-fobjc-weak", "-fobjc-arc"]

    objc_library(
        name = name,
        srcs = srcs,
        copts = copts + OBJC_COMPILER_FLAGS,
        target_compatible_with = ["@platforms//os:osx"],
        deps = deps,
        **kwargs
    )
