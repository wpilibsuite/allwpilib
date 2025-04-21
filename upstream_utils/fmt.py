#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root):
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/fmtlib/src",
        "src/main/native/thirdparty/fmtlib/include",
    ]:
        shutil.rmtree(os.path.join(wpiutil, d), ignore_errors=True)

    # Copy fmt source files into allwpilib
    walk_cwd_and_copy_if(
        lambda dp, f: dp.startswith(os.path.join(".", "src"))
        and f.endswith(".cc")
        and f != "fmt.cc",
        os.path.join(wpiutil, "src/main/native/thirdparty/fmtlib"),
    )

    # Copy fmt header files into allwpilib
    walk_cwd_and_copy_if(
        lambda dp, f: dp.startswith(os.path.join(".", "include", "fmt")),
        os.path.join(wpiutil, "src/main/native/thirdparty/fmtlib"),
    )


def main():
    name = "fmt"
    url = "https://github.com/fmtlib/fmt"
    tag = "11.1.0"

    fmt = Lib(name, url, tag, copy_upstream_src)
    fmt.main()


if __name__ == "__main__":
    main()
