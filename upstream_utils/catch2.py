#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, comment_out_invalid_includes, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root):
    upstream_root = os.path.abspath(".")
    catch2 = os.path.join(wpilib_root, "thirdparty/catch2")

    # Delete old install
    shutil.rmtree(
        os.path.join(catch2, "src/main/native"),
        ignore_errors=True,
    )
    shutil.rmtree(
        os.path.join(catch2, "cmake/"),
        ignore_errors=True,
    )

    # Copy Catch2 source files into allwpilib
    src_files = walk_cwd_and_copy_if(
        lambda dp, f: f == "Catch.cmake",
        os.path.join(catch2, "cmake/modules"),
    )
    src_files = walk_cwd_and_copy_if(
        lambda dp, f: f == "Clara.hpp",
        os.path.join(catch2, "src/main/native/include/third_party"),
    )
    os.chdir(os.path.join(upstream_root, "src"))
    src_files = walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".cpp") and dp.startswith("./catch2"),
        os.path.join(catch2, "src/main/native/cpp"),
    )
    src_files = walk_cwd_and_copy_if(
        lambda dp, f: (f.endswith(".hpp") or f.endswith(".hpp.in"))
        and (dp.startswith("./third_party") or dp.startswith("./catch2")),
        os.path.join(catch2, "src/main/native/include"),
    )

    for f in src_files:
        comment_out_invalid_includes(
            f,
            [
                os.path.join(catch2, "src/main/native/include"),
            ],
        )


def main():
    name = "catch2"
    url = "https://github.com/catchorg/Catch2.git"
    tag = "0321d2fce328b5e2ad106a8230ff20e0d5bf5501"

    patch_options = {
        "ignore_whitespace": True,
    }

    catch2 = Lib(name, url, tag, copy_upstream_src, patch_options)
    catch2.main()


if __name__ == "__main__":
    main()
