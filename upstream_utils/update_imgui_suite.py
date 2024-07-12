#!/usr/bin/env python3

import os
import shutil

from upstream_utils import (
    get_repo_root,
    clone_repo,
    walk_if,
    git_am,
    walk_cwd_and_copy_if,
)


def matches(dp, f, allowed_files):
    path = os.path.join(dp, f)
    return path in allowed_files


def walk_and_copy_if_matches(allowed_files, output_directory):
    walk_cwd_and_copy_if(
        lambda dp, f: matches(dp, f, allowed_files),
        output_directory,
    )


def update_imgui():
    upstream_root = clone_repo(
        "https://github.com/ocornut/imgui.git",
        "64b1e448d20c9be9275af731c34b4c7bf14a8e95",
        shallow=False,
    )
    wpilib_root = get_repo_root()
    imgui = os.path.join(wpilib_root, "thirdparty", "imgui_suite", "imgui")

    os.chdir(upstream_root)

    # Delete old install
    for d in ["include", "cpp"]:
        shutil.rmtree(os.path.join(imgui, d), ignore_errors=True)

    hdr_allow_list = [
        "./imgui.h",
        "./imstb_truetype.h",
        "./imgui_internal.h",
        "./imstb_rectpack.h",
        "./imconfig.h",
        "./imstb_textedit.h",
        "./backends/imgui_impl_glfw.h",
        "./backends/imgui_impl_metal.h",
        "./backends/imgui_impl_opengl3.h",
        "./backends/imgui_impl_dx11.h",
        "./backends/imgui_impl_opengl3_loader.h",
        "./backends/imgui_impl_opengl2.h",
        "./misc/cpp/imgui_stdlib.h",
    ]

    src_allow_list = [
        "./backends/imgui_impl_dx11.cpp",
        "./backends/imgui_impl_glfw.cpp",
        "./backends/imgui_impl_metal.mm",
        "./backends/imgui_impl_opengl2.cpp",
        "./backends/imgui_impl_opengl3.cpp",
        "./imgui.cpp",
        "./imgui_draw.cpp",
        "./imgui_tables.cpp",
        "./imgui_widgets.cpp",
        "./misc/cpp/imgui_stdlib.cpp",
    ]

    walk_and_copy_if_matches(hdr_allow_list, os.path.join(imgui, "include"))
    walk_and_copy_if_matches(src_allow_list, os.path.join(imgui, "cpp"))


def update_implot():
    upstream_root = clone_repo(
        "https://github.com/epezent/implot.git",
        "18c72431f8265e2b0b5378a3a73d8a883b2175ff",
        shallow=False,
    )
    wpilib_root = get_repo_root()
    implot = os.path.join(wpilib_root, "thirdparty", "imgui_suite", "implot")

    # Apply patches to upstream Git repo
    os.chdir(upstream_root)
    for f in [
        "0001-Supress-compiler-warnings.patch",
    ]:
        git_am(
            os.path.join(wpilib_root, "upstream_utils/imgui_suite_patches/implot", f)
        )

    # Delete old install
    for d in ["include", "cpp"]:
        shutil.rmtree(os.path.join(implot, d), ignore_errors=True)

    # Copy files
    walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".h"),
        os.path.join(implot, "include"),
    )
    walk_and_copy_if_matches(
        ["./implot_items.cpp", "./implot.cpp"], os.path.join(implot, "cpp")
    )


def update_glfw():
    upstream_root = clone_repo(
        "https://github.com/glfw/glfw.git",
        "6b57e08bb0078c9834889eab871bac2368198c15",
        shallow=False,
    )
    wpilib_root = get_repo_root()
    glfw = os.path.join(wpilib_root, "thirdparty", "imgui_suite", "glfw")

    # Apply patches to upstream Git repo
    os.chdir(upstream_root)
    for f in [
        "0001-Suppress-Compiler-Warnings.patch",
    ]:
        git_am(os.path.join(wpilib_root, "upstream_utils/imgui_suite_patches/glfw", f))

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

        if dp.startswith("./src"):
            return True

        return False

    src_files = walk_cwd_and_copy_if(
        src_filter,
        os.path.join(glfw),
    )

    # The upstream library automatically changes the files from .c -> .cpp. We want to keep it .c
    for f in src_files:
        if f.endswith(".cpp"):
            shutil.move(f, f[:-2])

    def cmake_filter(dp, f):
        if dp.startswith("./CMake"):
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


def update_stb():
    upstream_root = clone_repo(
        "https://github.com/nothings/stb.git",
        "c9064e317699d2e495f36ba4f9ac037e88ee371a",
        shallow=False,
    )
    wpilib_root = get_repo_root()
    stb = os.path.join(wpilib_root, "thirdparty", "imgui_suite", "stb")

    os.chdir(upstream_root)

    # Delete old install
    for d in ["include", "cpp"]:
        shutil.rmtree(os.path.join(stb, d), ignore_errors=True)

    # Copy files
    files = walk_cwd_and_copy_if(
        lambda dp, f: f == "stb_image.h",
        os.path.join(stb, "include"),
    )

    os.makedirs(os.path.join(stb, "cpp"))

    with open(os.path.join(stb, "cpp", "stb_image.cpp"), "w") as f:
        f.write(
            """#define STBI_WINDOWS_UTF8
#define STB_IMAGE_IMPLEMENTATION

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#include "stb_image.h"
"""
        )


def update_gl3w():
    upstream_root = clone_repo(
        "https://github.com/skaslev/gl3w",
        "5f8d7fd191ba22ff2b60c1106d7135bb9a335533",
        shallow=False,
    )
    wpilib_root = get_repo_root()
    gl3w = os.path.join(wpilib_root, "thirdparty", "imgui_suite", "gl3w")

    os.chdir(upstream_root)

    walk_cwd_and_copy_if(
        lambda dp, f: f == "gl3w_gen.py",
        os.path.join(gl3w),
    )


def main():
    original_dir = os.getcwd()

    update_imgui()
    os.chdir(original_dir)

    update_glfw()
    os.chdir(original_dir)

    update_implot()
    os.chdir(original_dir)

    update_stb()
    os.chdir(original_dir)

    update_gl3w()
    os.chdir(original_dir)


if __name__ == "__main__":
    main()
