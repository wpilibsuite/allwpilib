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
    implot = os.path.join(wpilib_root, "thirdparty", "imgui_suite", "implot")

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


def main():
    name = "implot"
    url = "https://github.com/epezent/implot.git"
    tag = "18c72431f8265e2b0b5378a3a73d8a883b2175ff"

    implot = Lib(name, url, tag, copy_upstream_src)
    implot.main()


if __name__ == "__main__":
    main()
