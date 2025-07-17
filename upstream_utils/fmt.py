#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib, has_prefix, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    wpiutil = wpilib_root / "wpiutil"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/fmtlib/src",
        "src/main/native/thirdparty/fmtlib/include",
    ]:
        shutil.rmtree(wpiutil / d, ignore_errors=True)

    # Copy fmt source files into allwpilib
    walk_cwd_and_copy_if(
        lambda dp, f: has_prefix(dp, Path("src"))
        and f.endswith(".cc")
        and f != "fmt.cc",
        wpiutil / "src/main/native/thirdparty/fmtlib",
    )

    # Copy fmt header files into allwpilib
    walk_cwd_and_copy_if(
        lambda dp, f: has_prefix(dp, Path("include/fmt")),
        wpiutil / "src/main/native/thirdparty/fmtlib",
    )


def main():
    name = "fmt"
    url = "https://github.com/fmtlib/fmt"
    # master on 2025-07-06
    tag = "300ce75ca610cc33cf51366572261bb974aada7b"

    fmt = Lib(name, url, tag, copy_upstream_src)
    fmt.main()


if __name__ == "__main__":
    main()
