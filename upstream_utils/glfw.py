#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, walk_cwd_and_copy_if


def matches(dp, f, allowed_files):
    path = os.path.join(dp, f)
    return path in allowed_files


def walk_and_copy_if_matches(allowed_files, output_directory):
    walk_cwd_and_copy_if(
        lambda dp, f: matches(dp, f, allowed_files),
        output_directory,
    )


def copy_upstream_src(wpilib_root):
    glfw = os.path.join(wpilib_root, "thirdparty", "imgui_suite", "glfw")

    # Delete old install
    for d in ["include", "src", "CMake"]:
        shutil.rmtree(os.path.join(glfw, d), ignore_errors=True)

    hdr_allow_list = [
        "./include/GLFW/glfw3.h",
        "./include/GLFW/glfw3native.h",
    ]

    walk_and_copy_if_matches(hdr_allow_list, glfw)

    def src_filter(dp, f):
        if f.endswith("CMakeLists.txt"):
            return False

        if dp.startswith(os.path.join(".", "src")):
            return True

        return False

    src_files = walk_cwd_and_copy_if(
        src_filter,
        os.path.join(glfw),
    )

    def cmake_filter(dp, f):
        if dp.startswith(os.path.join(".", "CMake")):
            return True

        path = os.path.join(dp, f)
        if path in ["./src/CMakeLists.txt", "./CMakeLists.txt"]:
            return True

        return False

    # Copy CMAKE files
    walk_cwd_and_copy_if(
        cmake_filter,
        os.path.join(glfw),
    )


def main():
    name = "glfw"
    url = "https://github.com/glfw/glfw.git"
    tag = "6b57e08bb0078c9834889eab871bac2368198c15"

    glfw = Lib(name, url, tag, copy_upstream_src)
    glfw.main()


if __name__ == "__main__":
    main()
