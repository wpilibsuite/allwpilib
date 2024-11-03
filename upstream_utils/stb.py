#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root):
    stb = os.path.join(wpilib_root, "thirdparty", "imgui_suite", "stb")

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


def main():
    name = "stb"
    url = "https://github.com/nothings/stb.git"
    tag = "c9064e317699d2e495f36ba4f9ac037e88ee371a"

    stb = Lib(name, url, tag, copy_upstream_src)
    stb.main()


if __name__ == "__main__":
    main()
