#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib


def copy_upstream_src(wpilib_root):
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    # Copy header into allwpilib
    dest_filename = os.path.join(
        wpiutil,
        f"src/main/native/thirdparty/argparse/include/wpi/argparse.h",
    )
    shutil.copyfile("include/argparse/argparse.hpp", dest_filename)
    # Rename namespace from argparse to wpi
    with open(dest_filename) as f:
        content = f.read()
    content = content.replace("namespace argparse", "namespace wpi")
    content = content.replace("argparse::", "wpi::")
    content = content.replace("ARGPARSE_", "WPI_")
    with open(dest_filename, "w") as f:
        f.write(content)


def main():
    name = "argparse_lib"
    url = "https://github.com/p-ranav/argparse"
    # master on 2024-09-11
    tag = "fd13c2859131ab463e617a5a8abcc69eb7e1d897"

    expected = Lib(name, url, tag, copy_upstream_src)
    expected.main()


if __name__ == "__main__":
    main()
