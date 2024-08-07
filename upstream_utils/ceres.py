#!/usr/bin/env python3

import os
import shutil

from upstream_utils import (
    comment_out_invalid_includes,
    walk_cwd_and_copy_if,
    Lib,
)


def copy_upstream_src(wpilib_root):
    wpimath = os.path.join(wpilib_root, "wpimath")

    # Delete old install
    shutil.rmtree(
        os.path.join(wpimath, "src/main/native/thirdparty/ceres"),
        ignore_errors=True,
    )
    shutil.rmtree(
        os.path.join(wpimath, "src/main/include/thirdparty/ceres"),
        ignore_errors=True,
    )

    # Copy ceres source files into allwpilib
    src_files = walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".cc"),
        os.path.join(wpimath, "src/main/native/thirdparty/ceres/src"),
    )

    # Copy ceres header files into allwpilib
    walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".h") and not dp.startswith("./examples"),
        os.path.join(wpimath, "src/main/native/thirdparty/ceres/include"),
    )

    for f in src_files:
        comment_out_invalid_includes(
            f,
            [
                os.path.join(wpimath, "src/main/native/thirdparty/ceres/include"),
            ],
        )


def main():
    name = "ceres"
    url = "https://github.com/ceres-solver/ceres-solver.git"
    tag = "95031308300389988109f45125b27b838548b0b4"

    patch_options = {
        "ignore_whitespace": True,
    }

    ceres = Lib(name, url, tag, copy_upstream_src, patch_options)
    ceres.main()


if __name__ == "__main__":
    main()
