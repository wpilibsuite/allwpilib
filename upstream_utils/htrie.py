#!/usr/bin/env python3

import os
import re
import shutil

from pathlib import Path
from upstream_utils import Lib, walk_if


def copy_upstream_src(wpilib_root: Path):
    htrie = wpilib_root / "wpiutil/src/main/native/thirdparty/htrie"

    # Delete old install
    shutil.rmtree(htrie / "include", ignore_errors=True)

    # Create lists of source and destination files
    os.chdir("include/tsl")
    files = walk_if(Path("."), lambda dp, f: True)
    src_include_files = [f.absolute() for f in files]
    wpiutil_htrie_root = htrie / "include/wpi/util"
    dest_include_files = [(wpiutil_htrie_root / f).with_suffix(".hpp") for f in files]

    # Copy htrie header files into allwpilib
    for i in range(len(src_include_files)):
        dest_dir = dest_include_files[i].parent
        if not dest_dir.exists():
            dest_dir.mkdir(parents=True)
        shutil.copyfile(src_include_files[i], dest_include_files[i])

    for include_file in dest_include_files:
        print(include_file)
        with open(include_file) as f:
            content = f.read()

        # Rename namespace from tsl to wpi
        content = content.replace("namespace tsl", "namespace wpi::util")
        content = content.replace("tsl::", "wpi::util::")

        # Fix include guards and other #defines
        content = content.replace("TSL_", "WPI_")

        # Handle move from .h -> .hpp
        content = re.sub(
            '#include "array-hash/array_(.*).h"', r'#include "array-hash/array_\1.hpp"', content
        )
        content = re.sub(
            '#include "array_(.*).h"', r'#include "array_\1.hpp"', content
        )
        content = re.sub(
            '#include "htrie_(.*).h"', r'#include "htrie_\1.hpp"', content
        )

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
