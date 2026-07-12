#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib, has_prefix, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    wpimath = wpilib_root / "wpimath"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/gcem/include",
    ]:
        shutil.rmtree(wpimath / d, ignore_errors=True)

    # Copy gcem include files into allwpilib
    walk_cwd_and_copy_if(
        lambda dp, f: has_prefix(dp, Path("include")),
        wpimath / "src/main/native/thirdparty/gcem",
    )


def main():
    name = "gcem"
    url = "https://github.com/kthohr/gcem.git"
    tag = "v1.18.0"

    gcem = Lib(name, url, tag, copy_upstream_src)
    gcem.main()


if __name__ == "__main__":
    main()
