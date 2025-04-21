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
    imgui = os.path.join(wpilib_root, "thirdparty", "imgui_suite", "imgui")

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
        "./imgui_demo.cpp",
        "./imgui_draw.cpp",
        "./imgui_tables.cpp",
        "./imgui_widgets.cpp",
        "./misc/cpp/imgui_stdlib.cpp",
    ]

    walk_and_copy_if_matches(hdr_allow_list, os.path.join(imgui, "include"))
    walk_and_copy_if_matches(src_allow_list, os.path.join(imgui, "cpp"))


def main():
    name = "imgui"
    url = "https://github.com/ocornut/imgui.git"
    tag = "64b1e448d20c9be9275af731c34b4c7bf14a8e95"

    imgui = Lib(name, url, tag, copy_upstream_src)
    imgui.main()


if __name__ == "__main__":
    main()
