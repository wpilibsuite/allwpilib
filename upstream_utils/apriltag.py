#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, comment_out_invalid_includes, walk_cwd_and_copy_if


def remove_tag(f: str):
    if "apriltag" in f:
        return False
    if "36h11" in f:
        return False
    if "16h5" in f:
        return False
    if "tag" in f:
        return True
    return False


def copy_upstream_src(wpilib_root):
    apriltag = os.path.join(wpilib_root, "apriltag")

    # Delete old install
    shutil.rmtree(
        os.path.join(apriltag, "src/main/native/thirdparty/apriltag"),
        ignore_errors=True,
    )
    shutil.rmtree(
        os.path.join(apriltag, "src/main/include/thirdparty/apriltag"),
        ignore_errors=True,
    )

    # Copy apriltag source files into allwpilib
    src_files = walk_cwd_and_copy_if(
        lambda dp, f: (f.endswith(".c") or f.endswith(".cpp"))
        and not dp.startswith(os.path.join(".", "example"))
        and not dp.startswith(os.path.join(".", "test"))
        and not f.endswith("getopt.c")
        and not "py" in f
        and not remove_tag(f),
        os.path.join(apriltag, "src/main/native/thirdparty/apriltag/src"),
    )

    # Copy apriltag header files into allwpilib
    walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".h")
        and not f.endswith("getopt.h")
        and not f.endswith("postscript_utils.h")
        and not remove_tag(f),
        os.path.join(apriltag, "src/main/native/thirdparty/apriltag/include"),
    )

    for f in src_files:
        comment_out_invalid_includes(
            f,
            [
                os.path.join(apriltag, "src/main/native/thirdparty/apriltag/include"),
                os.path.join(
                    apriltag, "src/main/native/thirdparty/apriltag/include/common"
                ),
            ],
        )


def main():
    name = "apriltag"
    url = "https://github.com/AprilRobotics/apriltag.git"
    tag = "3806edf38ac4400153677e510c9f9dcb81f472c8"

    patch_options = {
        "ignore_whitespace": True,
    }

    apriltag = Lib(name, url, tag, copy_upstream_src, patch_options)
    apriltag.main()


if __name__ == "__main__":
    main()
