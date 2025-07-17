#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib, has_prefix, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    glfw = wpilib_root / "thirdparty/imgui_suite/glfw"

    # Delete old install
    for d in ["include", "src", "CMake"]:
        shutil.rmtree(glfw / d, ignore_errors=True)

    hdr_allow_list = [
        Path("include/GLFW/glfw3.h"),
        Path("include/GLFW/glfw3native.h"),
    ]

    walk_cwd_and_copy_if(
        lambda dp, f: dp / f in hdr_allow_list,
        glfw,
    )

    def src_filter(dp: Path, f: str):
        if f == "CMakeLists.txt":
            return False

        if has_prefix(dp, Path("src")):
            return True

        return False

    src_files = walk_cwd_and_copy_if(
        src_filter,
        glfw,
    )

    def cmake_filter(dp: Path, f: str):
        if has_prefix(dp, Path("CMake")):
            return True

        if dp / f in [Path("src/CMakeLists.txt"), Path("CMakeLists.txt")]:
            return True

        return False

    # Copy CMAKE files
    walk_cwd_and_copy_if(
        cmake_filter,
        glfw,
    )


def main():
    name = "glfw"
    url = "https://github.com/glfw/glfw.git"
    # master on 2022-08-22
    tag = "6b57e08bb0078c9834889eab871bac2368198c15"

    glfw = Lib(name, url, tag, copy_upstream_src)
    glfw.main()


if __name__ == "__main__":
    main()
