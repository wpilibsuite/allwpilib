#!/usr/bin/env python3

import os
import re
import shutil
from pathlib import Path

from upstream_utils import Lib, walk_if


def copy_upstream_src(wpilib_root: Path):
    upstream_root = Path(".").absolute()
    wpimath = wpilib_root / "wpimath"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/units/include",
    ]:
        shutil.rmtree(wpimath / d, ignore_errors=True)

    # Copy units include files into allwpilib
    os.chdir(upstream_root / "include")
    files = walk_if(Path("."), lambda dp, f: True)
    src_include_files = [f.absolute() for f in files]
    wpimath_units_root = wpimath / "src/main/native/thirdparty/units/include/wpi"
    dest_include_files = [(wpimath_units_root / f).with_suffix(".hpp") for f in files]

    # Rename to .hpp
    for i in range(len(src_include_files)):
        dest_dir = dest_include_files[i].parent
        if not dest_dir.exists():
            dest_dir.mkdir(parents=True)
        shutil.copyfile(src_include_files[i], dest_include_files[i])

    # Perform namespace renames
    for wpi_file in dest_include_files:
        content: str
        with open(wpi_file) as f:
            content = f.read()

        content = content.replace("units::", "wpi::units::")
        content = content.replace("namespace units", "namespace wpi::units")
        content = re.sub(
            "#include <units/(.*)\\.h", r'#include <wpi/units/\1.hpp', content
        )

        with open(wpi_file, "w") as f:
            f.write(content)


def main():
    name = "units"
    url = "https://github.com/nholthaus/units.git"
    tag = "v3.6.0"

    units = Lib(name, url, tag, copy_upstream_src)
    units.main()


if __name__ == "__main__":
    main()
