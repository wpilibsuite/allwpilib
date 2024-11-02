#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, walk_cwd_and_copy_if, walk_if

EXCLUDED_FILES = [
    "gtest_main.cc",
    "gtest-all.cc",
    "gmock_main.cc",
    "gmock-all.cc",
    "README.md",
]


def walk_and_remap_copy(third_party_root, include_prefix):
    gmock_files = walk_if(
        ".", lambda dp, f: include_prefix in dp and f not in EXCLUDED_FILES
    )

    for f in gmock_files:
        dst_file = os.path.join(
            third_party_root, "include", f[len(include_prefix) + 1 :]
        )
        dest_dir = os.path.dirname(dst_file)
        if not os.path.exists(dest_dir):
            os.makedirs(dest_dir)
        shutil.copyfile(f, dst_file)


def copy_upstream_src(wpilib_root):
    third_party_root = os.path.join(wpilib_root, "thirdparty/googletest")

    # Delete old install
    for d in [
        "include",
        "src",
    ]:
        shutil.rmtree(os.path.join(third_party_root, d), ignore_errors=True)

    walk_cwd_and_copy_if(
        lambda dp, f: "googlemock/src" in dp and f not in EXCLUDED_FILES,
        os.path.join(third_party_root, "src"),
    )

    walk_cwd_and_copy_if(
        lambda dp, f: "googletest/src" in dp and f not in EXCLUDED_FILES,
        os.path.join(third_party_root, "src"),
    )

    walk_and_remap_copy(
        third_party_root,
        "./googlemock/include",
    )

    walk_and_remap_copy(
        third_party_root,
        "./googletest/include",
    )


def main():
    name = "googletest"
    url = "https://github.com/google/googletest.git"
    tag = "v1.14.0"

    googletest = Lib(name, url, tag, copy_upstream_src)
    googletest.main()


if __name__ == "__main__":
    main()
