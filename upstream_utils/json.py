#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib


def copy_upstream_src(wpilib_root: Path):
    wpiutil = wpilib_root / "wpiutil"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/json/cpp",
        "src/main/native/thirdparty/json/include",
    ]:
        shutil.rmtree(wpiutil / d, ignore_errors=True)

    # Create lists of source and destination files
    src_include_files = ["json.h"]
    src_src_files = ["json.cpp", "jtckdint.h"]
    wpiutil_json_root = wpiutil / "src/main/native/thirdparty/json/include/wpi/util"
    dest_include_files = [wpiutil_json_root / "json.hpp"]

    # Copy json header files into allwpilib
    for i in range(len(src_include_files)):
        dest_dir = dest_include_files[i].parent
        if not dest_dir.exists():
            dest_dir.mkdir(parents=True)
        shutil.copyfile(src_include_files[i], dest_include_files[i])

    # Copy json src files into allwpilib
    for i in range(len(src_src_files)):
        dest_dir = wpiutil / "src/main/native/thirdparty/json/src"
        if not dest_dir.exists():
            dest_dir.mkdir(parents=True)
        shutil.copyfile(src_src_files[i], dest_dir / src_src_files[i])


def main():
    name = "json"
    url = "https://github.com/jart/json.cpp.git"
    tag = "6ec4e44a5bbaadbe677473378c3d2133644c58a1"

    patch_options = {
        "use_threeway": True,
    }

    json = Lib(name, url, tag, copy_upstream_src, patch_options)
    json.main()


if __name__ == "__main__":
    main()
