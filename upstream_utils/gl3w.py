#!/usr/bin/env python3

import os

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root):
    gl3w = os.path.join(wpilib_root, "thirdparty", "imgui_suite", "gl3w")

    walk_cwd_and_copy_if(
        lambda dp, f: f == "gl3w_gen.py",
        os.path.join(gl3w),
    )


def main():
    name = "gl3w"
    url = "https://github.com/skaslev/gl3w"
    tag = "5f8d7fd191ba22ff2b60c1106d7135bb9a335533"

    gl3w = Lib(name, url, tag, copy_upstream_src)
    gl3w.main()


if __name__ == "__main__":
    main()
