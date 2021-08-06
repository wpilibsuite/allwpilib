#!/usr/bin/env python3

import os
import shutil

from upstream_utils import setup_upstream_repo, comment_out_invalid_includes, walk_cwd_and_copy_if


def main():
    root, repo = setup_upstream_repo("https://github.com/fmtlib/fmt", "8.0.1")
    wpiutil = os.path.join(root, "wpiutil")

    # Delete old install
    for d in ["src/main/native/fmtlib/src", "src/main/native/fmtlib/include"]:
        shutil.rmtree(os.path.join(wpiutil, d), ignore_errors=True)

    # Copy fmt source files into allwpilib
    src_files = walk_cwd_and_copy_if(
        lambda dp, f: dp.endswith("src") and f.endswith(".cc") and f !=
        "fmt.cc", os.path.join(wpiutil, "src/main/native/fmtlib"))

    # Copy fmt header files into allwpilib
    include_files = walk_cwd_and_copy_if(
        lambda dp, f: dp.endswith("include/fmt"),
        os.path.join(wpiutil, "src/main/native/fmtlib"))

    for f in src_files:
        comment_out_invalid_includes(
            f, [os.path.join(wpiutil, "src/main/native/fmtlib/include")])
    for f in include_files:
        comment_out_invalid_includes(
            f, [os.path.join(wpiutil, "src/main/native/fmtlib/include")])


if __name__ == "__main__":
    main()
