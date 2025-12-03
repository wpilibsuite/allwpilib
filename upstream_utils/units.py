#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib, has_prefix, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    wpimath = wpilib_root / "wpimath"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/units/include",
    ]:
        shutil.rmtree(wpimath / d, ignore_errors=True)

    # Copy units include files into allwpilib
    walk_cwd_and_copy_if(
        lambda dp, f: has_prefix(dp, Path("include/units")),
        wpimath / "src/main/native/thirdparty/units",
    )


def main():
    name = "units"
    url = "https://github.com/nholthaus/units.git"
    tag = "v3.2.0"

    units = Lib(name, url, tag, copy_upstream_src)
    units.main()


if __name__ == "__main__":
    main()
