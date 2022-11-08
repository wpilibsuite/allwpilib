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


def main():
    upstream_root = clone_repo("https://github.com/fmtlib/fmt", "9.1.0")
    wpilib_root = get_repo_root()
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    # Apply patches to upstream Git repo
    os.chdir(upstream_root)
    for f in [
        "0001-Don-t-throw-on-write-failure.patch",
        "0002-Suppress-C-20-clang-tidy-warning-false-positive.patch",
    ]:
        git_am(os.path.join(wpilib_root, "upstream_utils/fmt_patches", f))

    # Delete old install
    for d in [
        "src/main/native/thirdparty/fmtlib/src",
        "src/main/native/thirdparty/fmtlib/include",
    ]:
        shutil.rmtree(os.path.join(wpiutil, d), ignore_errors=True)

    # Copy fmt source files into allwpilib
    src_files = walk_cwd_and_copy_if(
        lambda dp, f: dp.endswith("src") and f.endswith(".cc") and f != "fmt.cc",
        os.path.join(wpiutil, "src/main/native/thirdparty/fmtlib"),
    )

    # Copy fmt header files into allwpilib
    include_files = walk_cwd_and_copy_if(
        lambda dp, f: dp.endswith("include/fmt"),
        os.path.join(wpiutil, "src/main/native/thirdparty/fmtlib"),
    )

    for f in src_files:
        comment_out_invalid_includes(
            f, [os.path.join(wpiutil, "src/main/native/thirdparty/fmtlib/include")]
        )
    for f in include_files:
        comment_out_invalid_includes(
            f, [os.path.join(wpiutil, "src/main/native/thirdparty/fmtlib/include")]
        )


if __name__ == "__main__":
    main()
