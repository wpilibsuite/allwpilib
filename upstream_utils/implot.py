#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    implot = wpilib_root / "thirdparty/imgui_suite/implot"

    # Delete old install
    for d in ["include", "cpp"]:
        shutil.rmtree(implot / d, ignore_errors=True)

    # Copy files
    walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".h"),
        implot / "include",
    )
    implot_files = [Path("implot_items.cpp"), Path("implot.cpp")]
    walk_cwd_and_copy_if(
        lambda dp, f: dp / f in implot_files,
        implot / "cpp",
    )


def main():
    name = "implot"
    url = "https://github.com/epezent/implot.git"
    tag = "18c72431f8265e2b0b5378a3a73d8a883b2175ff"

    implot = Lib(name, url, tag, copy_upstream_src)
    implot.main()


if __name__ == "__main__":
    main()
