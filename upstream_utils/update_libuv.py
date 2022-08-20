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
    upstream_root = clone_repo("https://github.com/libuv/libuv", "v1.44.2")
    wpilib_root = get_repo_root()
    wpinet = os.path.join(wpilib_root, "wpinet")

    # Apply patches to upstream Git repo
    os.chdir(upstream_root)
    for f in [
        "0001-Fix-missing-casts.patch",
        "0002-Fix-warnings.patch",
        "0003-Preprocessor-cleanup.patch",
        "0004-Cleanup-problematic-language.patch",
        "0005-Use-roborio-time.patch",
        "0006-Style-comments-cleanup.patch",
        "0007-Squelch-GCC-12.1-warnings.patch",
        "0008-Fix-Win32-warning-suppression-pragma.patch",
        "0009-Avoid-unused-variable-warning-on-Mac.patch",
    ]:
        git_am(os.path.join(wpilib_root, "upstream_utils/libuv_patches", f))

    # Delete old install
    for d in ["src/main/native/thirdparty/libuv"]:
        shutil.rmtree(os.path.join(wpinet, d), ignore_errors=True)

    include_ignorelist = [
        "aix.h",
        "os390.h",
        "stdint-msvc2008.h",
        "sunos.h",
    ]

    include_files = walk_cwd_and_copy_if(
        lambda dp, f: "include" in dp and f not in include_ignorelist,
        os.path.join(wpinet, "src/main/native/thirdparty/libuv"),
    )

    src_ignorelist = [
        "aix-common.c",
        "aix.c",
        "bsd-proctitle.c",
        "darwin-stub.c",
        "haiku.c",
        "hurd.c",
        "os390-proctitle.c",
        "os390-syscalls.c",
        "os390-syscalls.h",
        "os390.c",
        "qnx.c",
        "sunos.c",
        "sysinfo-loadavg.c",
        "sysinfo-memory.c",
    ]
    src_files = walk_cwd_and_copy_if(
        lambda dp, f: "src" in dp and "docs" not in dp and f not in src_ignorelist,
        os.path.join(wpinet, "src/main/native/thirdparty/libuv"),
    )


if __name__ == "__main__":
    main()
