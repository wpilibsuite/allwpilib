#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib, has_prefix, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    wpinet = wpilib_root / "wpinet"

    # Delete old install
    for d in ["src/main/native/thirdparty/libuv"]:
        shutil.rmtree(wpinet / d, ignore_errors=True)

    include_ignorelist = [
        "aix.h",
        "os390.h",
        "stdint-msvc2008.h",
        "sunos.h",
    ]

    walk_cwd_and_copy_if(
        lambda dp, f: has_prefix(dp, Path("include")) and f not in include_ignorelist,
        wpinet / "src/main/native/thirdparty/libuv",
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
    walk_cwd_and_copy_if(
        lambda dp, f: has_prefix(dp, Path("src")) and f not in src_ignorelist,
        wpinet / "src/main/native/thirdparty/libuv",
        rename_c_to_cpp=True,
    )


def main():
    name = "libuv"
    url = "https://github.com/libuv/libuv"
    tag = "v1.49.2"

    libuv = Lib(name, url, tag, copy_upstream_src)
    libuv.main()


if __name__ == "__main__":
    main()
