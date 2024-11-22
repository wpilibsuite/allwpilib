#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root):
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/debugging/src",
        "src/main/native/thirdparty/debugging/include",
    ]:
        shutil.rmtree(os.path.join(wpiutil, d), ignore_errors=True)

    # Copy debugging files into allwpilib
    filenames = walk_cwd_and_copy_if(
        lambda dp, f: dp.startswith(os.path.join(".", "src"))
        or dp.startswith(os.path.join(".", "include")),
        os.path.join(wpiutil, "src/main/native/thirdparty/debugging"),
    )

    for filename in filenames:
        with open(filename) as f:
            content = f.read()

        # Rename namespace from stdx to wpi
        content = content.replace("namespace stdx", "namespace wpi")

        with open(filename, "w") as f:
            f.write(content)


def main():
    name = "debugging"
    url = "https://github.com/grafikrobot/debugging"
    # master on 2024-11-21
    tag = "c510133c44894b93afbb5be55275bfb88163a2cb"

    expected = Lib(name, url, tag, copy_upstream_src)
    expected.main()


if __name__ == "__main__":
    main()
