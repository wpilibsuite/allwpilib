#!/usr/bin/env python3

import os
import shutil
from pathlib import Path

from upstream_utils import Lib, walk_if


def copy_upstream_src(wpilib_root: Path):
    wpiutil = wpilib_root / "wpiutil"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/json/include",
    ]:
        shutil.rmtree(wpiutil / d, ignore_errors=True)

    # Create lists of source and destination files
    os.chdir("include/nlohmann")
    files = walk_if(Path("."), lambda dp, f: True)
    src_include_files = [f.absolute() for f in files]
    wpiutil_json_root = wpiutil / "src/main/native/thirdparty/json/include/wpi/util"
    dest_include_files = [wpiutil_json_root / f for f in files]

    # Copy json header files into allwpilib
    for i in range(len(src_include_files)):
        dest_dir = dest_include_files[i].parent
        if not dest_dir.exists():
            dest_dir.mkdir(parents=True)
        shutil.copyfile(src_include_files[i], dest_include_files[i])

    for include_file in dest_include_files:
        with open(include_file) as f:
            content = f.read()

        # Rename namespace from nlohmann to wpi
        content = content.replace("namespace nlohmann", "namespace wpi")
        content = content.replace("nlohmann::", "wpi::")

        # Fix internal includes
        content = content.replace("include <nlohmann/", "include <wpi/util/")

        # Fix include guards and other #defines
        content = content.replace("NLOHMANN_", "WPI_")

        with open(include_file, "w") as f:
            f.write(content)


def main():
    name = "json"
    url = "https://github.com/nlohmann/json"
    tag = "v3.11.3"

    patch_options = {
        "use_threeway": True,
    }

    json = Lib(name, url, tag, copy_upstream_src, patch_options)
    json.main()


if __name__ == "__main__":
    main()
