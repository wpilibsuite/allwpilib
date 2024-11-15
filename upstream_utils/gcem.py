#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root):
    wpimath = os.path.join(wpilib_root, "wpimath")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/gcem/include",
    ]:
        shutil.rmtree(os.path.join(wpimath, d), ignore_errors=True)

    # Copy gcem include files into allwpilib
    walk_cwd_and_copy_if(
        lambda dp, f: dp.startswith(os.path.join(".", "include")),
        os.path.join(wpimath, "src/main/native/thirdparty/gcem"),
    )


def main():
    name = "gcem"
    url = "https://github.com/kthohr/gcem.git"
    tag = "v1.18.0"

    gcem = Lib(name, url, tag, copy_upstream_src)
    gcem.main()


if __name__ == "__main__":
    main()
