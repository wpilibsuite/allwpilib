#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, walk_if


def copy_upstream_src(wpilib_root):
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/json/include",
    ]:
        shutil.rmtree(os.path.join(wpiutil, d), ignore_errors=True)

    # Create lists of source and destination files
    os.chdir("include/nlohmann")
    files = walk_if(".", lambda dp, f: True)
    src_include_files = [os.path.abspath(f) for f in files]
    wpiutil_json_root = os.path.join(
        wpiutil, "src/main/native/thirdparty/json/include/wpi"
    )
    dest_include_files = [
        os.path.join(wpiutil_json_root, f.replace(".hpp", ".h")) for f in files
    ]

    # Copy json header files into allwpilib
    for i in range(len(src_include_files)):
        dest_dir = os.path.dirname(dest_include_files[i])
        if not os.path.exists(dest_dir):
            os.makedirs(dest_dir)
        shutil.copyfile(src_include_files[i], dest_include_files[i])

    for include_file in dest_include_files:
        with open(include_file) as f:
            content = f.read()

        # Rename namespace from nlohmann to wpi
        content = content.replace("namespace nlohmann", "namespace wpi")
        content = content.replace("nlohmann::", "wpi::")

        # Fix internal includes
        content = content.replace(".hpp>", ".h>")
        content = content.replace("include <nlohmann/", "include <wpi/")

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
