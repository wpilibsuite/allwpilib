#!/usr/bin/env python3

import os
import shutil

from upstream_utils import (
    get_repo_root,
    clone_repo,
    comment_out_invalid_includes,
    walk_cwd_and_copy_if,
    git_am,
)


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


def main():
    upstream_root = clone_repo(
        "https://github.com/AprilRobotics/apriltag.git",
        "ebdb2017e04b8e36f7d8a12ce60060416a905e12",
        shallow=False,
    )
    wpilib_root = get_repo_root()
    apriltag = os.path.join(wpilib_root, "apriltag")

    # Apply patches to upstream Git repo
    os.chdir(upstream_root)
    # for f in [
    #     "0001-apriltag_pose.c-Set-NULL-when-second-solution-could-.patch",
    #     "0002-zmaxheapify-Avoid-return-of-void-expression.patch",
    #     "0003-Avoid-unused-variable-warnings-in-release-builds.patch",
    #     "0004-Make-orthogonal_iteration-exit-early-upon-convergenc.patch",
    #     "0005-Fix-early-exit-3.patch",
    #     "0006-Fix-clang-16-warnings-4.patch",
    #     "0007-Fix-signed-left-shift-warning.patch",
    #     "0008-Avoid-incompatible-pointer-warning.patch",
    #     "0009-Fix-clang-16-warnings-in-more-files-5.patch",
    #     "0010-Fix-GCC-14-calloc-warning-7.patch",
    # ]:
    #     git_am(os.path.join(wpilib_root, "upstream_utils/apriltag_patches", f))

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
        lambda dp, f: (f.endswith(".c")
        or f.endswith(".cpp"))
        and not dp.startswith("./example")
        and not f == "getopt.cpp"
        and not "py" in f
        and not remove_tag(f),
        os.path.join(apriltag, "src/main/native/thirdparty/apriltag"),
    )

    # Copy apriltag header files into allwpilib
    include_files = walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".h")
        and not f == "getopt.h"
        and not f == "postscript_utils.h"
        and not remove_tag(f),
        os.path.join(apriltag, "src/main/include/thirdparty/apriltag"),
    )

    for f in src_files:
        comment_out_invalid_includes(
            f, [os.path.join(apriltag, "src/main/include/thirdparty/apriltag")]
        )
    for f in include_files:
        comment_out_invalid_includes(
            f, [os.path.join(apriltag, "src/main/include/thirdparty/apriltag")]
        )


if __name__ == "__main__":
    main()
