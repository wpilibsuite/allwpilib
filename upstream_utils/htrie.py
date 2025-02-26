#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, walk_if


def copy_upstream_src(wpilib_root):
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/htrie/include",
    ]:
        shutil.rmtree(os.path.join(wpiutil, d), ignore_errors=True)

    # Create lists of source and destination files
    os.chdir("include/tsl")
    files = walk_if(".", lambda dp, f: True)
    src_include_files = [os.path.abspath(f) for f in files]
    wpiutil_htrie_root = os.path.join(
        wpiutil, "src/main/native/thirdparty/htrie/include/wpi"
    )
    dest_include_files = [os.path.join(wpiutil_htrie_root, f) for f in files]

    # Copy htrie header files into allwpilib
    for i in range(len(src_include_files)):
        dest_dir = os.path.dirname(dest_include_files[i])
        if not os.path.exists(dest_dir):
            os.makedirs(dest_dir)
        shutil.copyfile(src_include_files[i], dest_include_files[i])

    for include_file in dest_include_files:
        with open(include_file) as f:
            content = f.read()

        # Rename namespace from tsl to wpi
        content = content.replace("namespace tsl", "namespace wpi")
        content = content.replace("tsl::", "wpi::")

        # Fix include guards and other #defines
        content = content.replace("TSL_", "WPI_")

        with open(include_file, "w") as f:
            f.write(content)


def main():
    name = "htrie"
    url = "https://github.com/Tessil/hat-trie"
    # master on 2025-02-16
    tag = "25fdf359711eb27e9e7ec0cfe19cc459ec6488d7"

    expected = Lib(name, url, tag, copy_upstream_src)
    expected.main()


if __name__ == "__main__":
    main()
