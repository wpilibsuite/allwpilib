#!/usr/bin/env python3

import os
import shutil
from pathlib import Path

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    upstream_root = Path(".").absolute()
    benchmark = wpilib_root / "benchmark"

    # Delete old install
    shutil.rmtree(
        benchmark / "src/main/native/thirdparty/benchmark",
        ignore_errors=True,
    )

    # Copy benchmark source files into allwpilib
    os.chdir(upstream_root / "src")
    walk_cwd_and_copy_if(
        lambda dp, f: f != "benchmark_main.cc" and f != "CMakeLists.txt",
        benchmark / "src/main/native/thirdparty/benchmark/src",
    )

    # Copy benchmark header files into allwpilib
    os.chdir(upstream_root / "include")
    walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".h"),
        benchmark / "src/main/native/thirdparty/benchmark/include",
    )


def main():
    name = "benchmark"
    url = "https://github.com/google/benchmark.git"
    tag = "v1.9.4"

    benchmark = Lib(name, url, tag, copy_upstream_src)
    benchmark.main()


if __name__ == "__main__":
    main()
