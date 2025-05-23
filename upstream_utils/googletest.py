#!/usr/bin/env python3

import os
import shutil
from pathlib import Path

from upstream_utils import Lib, has_prefix, walk_cwd_and_copy_if

EXCLUDED_FILES = [
    "gtest_main.cc",
    "gtest-all.cc",
    "gmock_main.cc",
    "gmock-all.cc",
    "README.md",
]


def copy_upstream_src(wpilib_root: Path):
    upstream_root = Path(".").absolute()
    third_party_root = wpilib_root / "thirdparty/googletest"

    # Delete old install
    for d in [
        "include",
        "src",
    ]:
        shutil.rmtree(third_party_root / d, ignore_errors=True)

    walk_cwd_and_copy_if(
        lambda dp, f: has_prefix(dp, Path("googlemock/src"))
        and f not in EXCLUDED_FILES,
        third_party_root / "src",
    )

    walk_cwd_and_copy_if(
        lambda dp, f: has_prefix(dp, Path("googletest/src"))
        and f not in EXCLUDED_FILES,
        third_party_root / "src",
    )

    os.chdir(upstream_root / "googlemock/include")
    walk_cwd_and_copy_if(
        lambda dp, f: f not in EXCLUDED_FILES,
        third_party_root / "include",
    )

    os.chdir(upstream_root / "googletest/include")
    walk_cwd_and_copy_if(
        lambda dp, f: f not in EXCLUDED_FILES,
        third_party_root / "include",
    )


def main():
    name = "googletest"
    url = "https://github.com/google/googletest.git"
    tag = "v1.14.0"

    googletest = Lib(name, url, tag, copy_upstream_src)
    googletest.main()


if __name__ == "__main__":
    main()
