#!/usr/bin/env python3

import os
import shutil
from pathlib import Path

from upstream_utils import Lib, has_prefix, walk_cwd_and_copy_if


def fix_file(filename):
    with open(filename) as f:
        content = f.read()

    # Prepend wpi namespace
    content = content.replace(
        "namespace double_conversion", "namespace wpi::double_conversion"
    )

    # Change includes
    content = content.replace('include "', 'include "wpi/double-conversion/')

    with open(filename, "w") as f:
        f.write(content)


def copy_upstream_src(wpilib_root: Path):
    upstream_root = Path(".").absolute()
    third_party_root = (
        wpilib_root / "wpiutil/src/main/native/thirdparty/double-conversion"
    )

    # Delete old install
    for d in [
        "include",
        "src",
    ]:
        shutil.rmtree(third_party_root / d, ignore_errors=True)

    os.chdir(upstream_root / "double-conversion")
    filenames = walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".cc"),
        third_party_root / "src",
    )

    for filename in filenames:
        fix_file(filename)

    os.chdir(upstream_root)
    filenames = walk_cwd_and_copy_if(
        lambda dp, f: has_prefix(dp, Path("double-conversion")) and f.endswith(".h"),
        third_party_root / "include/wpi",
    )

    for filename in filenames:
        fix_file(filename)


def main():
    name = "double-conversion"
    url = "https://github.com/google/double-conversion.git"
    tag = "v3.4.0"

    double_conversion = Lib(name, url, tag, copy_upstream_src)
    double_conversion.main()


if __name__ == "__main__":
    main()
