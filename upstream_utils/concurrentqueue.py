#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib


def copy_upstream_src(wpilib_root):
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    # Copy header into allwpilib
    dest_filename = os.path.join(
        wpiutil, f"src/main/native/thirdparty/concurrentqueue/include/wpi/concurrentqueue.h"
    )
    shutil.copyfile("concurrentqueue.h", dest_filename)
    # Rename namespace from moodycamel to wpi
    with open(dest_filename) as f:
        content = f.read()
    content = content.replace("namespace moodycamel", "namespace wpi")
    content = content.replace("moodycamel::", "wpi::")
    # The upstream library has macros that would conflict with other thirdparty WPI_ macros
    content = content.replace("MOODYCAMEL_", "WPI_CQ_")
    with open(dest_filename, "w") as f:
        f.write(content)


def main():
    name = "concurrentqueue"
    url = "https://github.com/cameron314/concurrentqueue"
    # master on 2024-09-10
    tag = "6dd38b8a1dbaa7863aa907045f32308a56a6ff5d"

    expected = Lib(name, url, tag, copy_upstream_src)
    expected.main()


if __name__ == "__main__":
    main()
