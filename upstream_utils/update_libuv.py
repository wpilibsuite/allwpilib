#!/usr/bin/env python3

import os
import shutil
import re

from upstream_utils import setup_upstream_repo, comment_out_invalid_includes, walk_cwd_and_copy_if, am_patches, walk_if, copy_to


def main():
    root, repo = setup_upstream_repo("http://github.com/libuv/libuv", "v1.30.1")
    wpinet = os.path.join(root, "wpinet")

    # Apply patches to original git repo
    am_patches(repo, [
        # yapf: disable
        os.path.join(root, "upstream_utils/libuv_patches/0001-Fix-missing-casts.patch"),
        os.path.join(root, "upstream_utils/libuv_patches/0002-Fix-warnings.patch"),
        os.path.join(root, "upstream_utils/libuv_patches/0003-Preprocessor-cleanup.patch"),
        os.path.join(root, "upstream_utils/libuv_patches/0004-Cleanup-problematic-language.patch"),
        os.path.join(root, "upstream_utils/libuv_patches/0005-Use-roborio-time.patch"),
        os.path.join(root, "upstream_utils/libuv_patches/0006-Style-comments-cleanup.patch"),
        # yapf: enable
    ])

    # Delete old install
    for d in ["src/main/native/thirdparty/libuv"]:
        shutil.rmtree(os.path.join(wpinet, d), ignore_errors=True)

    include_blacklist = [
        "aix.h",
        "os390.h",
        "stdint-msvc2008.h",
        "sunos.h",
    ]

    include_files = walk_cwd_and_copy_if(
        lambda dp, f: "include" in dp and f not in include_blacklist,
        os.path.join(wpinet, "src/main/native/thirdparty/libuv"))

    src_blacklist = [
        "aix-common.c",
        "aix.c",
        "bsd-proctitle.c",
        "haiku.c",
        "os390-syscalls.c",
        "os390-syscalls.h",
        "os390.c",
        "sunos.c",
        "sysinfo-memory.c",
    ]
    src_files = walk_cwd_and_copy_if(
        lambda dp, f: "src" in dp and "docs" not in dp and f not in
        src_blacklist, os.path.join(wpinet, "src/main/native/thirdparty/libuv"))


if __name__ == "__main__":
    main()
