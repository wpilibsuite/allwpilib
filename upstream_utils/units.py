#!/usr/bin/env python3

import os
import shutil
from pathlib import Path

from upstream_utils import Lib, has_prefix, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    upstream_root: Path = Path(".").absolute()
    wpimath: Path = wpilib_root / "wpimath"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/units/include",
    ]:
        shutil.rmtree(wpimath / d, ignore_errors=True)

    # Copy units include files into allwpilib
    os.chdir(upstream_root / "include")
    wpi_files: list[Path] = walk_cwd_and_copy_if(
        lambda dp, f: has_prefix(dp, Path("units")),
        wpimath / "src/main/native/thirdparty/units/include/wpi",
    )

    # Perform namespace renames
    for wpi_file in wpi_files:
        content: str
        with open(wpi_file) as f:
            content = f.read()

        content = content.replace("units::", "wpi::units::")
        content = content.replace("namespace units", "namespace wpi::units")
        content = content.replace("#include <units/", "#include <wpi/units/")

        with open(wpi_file, "w") as f:
            f.write(content)


def main():
    name = "units"
    url = "https://github.com/nholthaus/units.git"
    tag = "v3.2.0"

    units = Lib(name, url, tag, copy_upstream_src)
    units.main()


if __name__ == "__main__":
    main()
