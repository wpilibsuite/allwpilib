#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib, walk_cwd_and_copy_if


def walk_and_copy_if_matches(allowed_files: list[Path], output_directory: Path):
    walk_cwd_and_copy_if(
        lambda dp, f: dp / f in allowed_files,
        output_directory,
    )


def copy_upstream_src(wpilib_root: Path):
    imgui = wpilib_root / "thirdparty/imgui_suite/imgui"

    # Delete old install
    for d in ["include", "cpp"]:
        shutil.rmtree(imgui / d, ignore_errors=True)

    hdr_allow_list = [
        Path("imgui.h"),
        Path("imstb_truetype.h"),
        Path("imgui_internal.h"),
        Path("imstb_rectpack.h"),
        Path("imconfig.h"),
        Path("imstb_textedit.h"),
        Path("backends/imgui_impl_glfw.h"),
        Path("backends/imgui_impl_metal.h"),
        Path("backends/imgui_impl_opengl3.h"),
        Path("backends/imgui_impl_dx11.h"),
        Path("backends/imgui_impl_opengl3_loader.h"),
        Path("backends/imgui_impl_opengl2.h"),
        Path("misc/cpp/imgui_stdlib.h"),
    ]

    src_allow_list = [
        Path("backends/imgui_impl_dx11.cpp"),
        Path("backends/imgui_impl_glfw.cpp"),
        Path("backends/imgui_impl_metal.mm"),
        Path("backends/imgui_impl_opengl2.cpp"),
        Path("backends/imgui_impl_opengl3.cpp"),
        Path("imgui.cpp"),
        Path("imgui_demo.cpp"),
        Path("imgui_draw.cpp"),
        Path("imgui_tables.cpp"),
        Path("imgui_widgets.cpp"),
        Path("misc/cpp/imgui_stdlib.cpp"),
    ]

    walk_and_copy_if_matches(hdr_allow_list, imgui / "include")
    walk_and_copy_if_matches(src_allow_list, imgui / "cpp")


def main():
    name = "imgui"
    url = "https://github.com/ocornut/imgui.git"
    tag = "64b1e448d20c9be9275af731c34b4c7bf14a8e95"

    imgui = Lib(name, url, tag, copy_upstream_src)
    imgui.main()


if __name__ == "__main__":
    main()
