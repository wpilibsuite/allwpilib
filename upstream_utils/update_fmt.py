#!/usr/bin/env python3

import os
import shutil

from upstream_utils import (
    setup_upstream_repo,
    comment_out_invalid_includes,
    walk_cwd_and_copy_if,
    am_patches,
)


def main():
    root, repo = setup_upstream_repo("https://github.com/fmtlib/fmt", "8.1.1")
    wpiutil = os.path.join(root, "wpiutil")

    # Apply patches to original git repo
    prefix = os.path.join(root, "upstream_utils/fmt_patches")
    am_patches(repo, [os.path.join(prefix, "0001-Don-t-throw-on-write-failure.patch")])

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
