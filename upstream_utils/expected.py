#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib


def copy_upstream_src(wpilib_root):
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    # Copy expected header into allwpilib
    dest_filename = os.path.join(
        wpiutil, "src/main/native/thirdparty/expected/include/wpi/expected"
    )
    shutil.copyfile("include/tl/expected.hpp", dest_filename)

    # Rename namespace from tl to wpi, and detail to detail_expected
    with open(dest_filename) as f:
        content = f.read()
    content = content.replace("namespace tl", "namespace wpi")
    content = content.replace("tl::", "wpi::")
    content = content.replace("TL_", "WPI_")
    content = content.replace("namespace detail", "namespace detail_expected")
    content = content.replace("detail::", "detail_expected::")
    with open(dest_filename, "w") as f:
        f.write(content)


def main():
    name = "expected"
    url = "https://github.com/TartanLlama/expected"
    # master on 2024-01-25
    tag = "3f0ca7b19253129700a073abfa6d8638d9f7c80c"

    expected = Lib(name, url, tag, copy_upstream_src)
    expected.main()


if __name__ == "__main__":
    main()
