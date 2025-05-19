#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib, has_prefix, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    wpimath = wpilib_root / "wpimath"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/sleipnir/src",
        "src/main/native/thirdparty/sleipnir/include",
    ]:
        shutil.rmtree(wpimath / d, ignore_errors=True)

    # Copy Sleipnir files into allwpilib
    walk_cwd_and_copy_if(
        lambda dp, f: (has_prefix(dp, Path("include")) or has_prefix(dp, Path("src")))
        and not f == "small_vector.hpp"
        or f == ".clang-format"
        or f == ".clang-tidy"
        or f == ".styleguide"
        or f == ".styleguide-license",
        wpimath / "src/main/native/thirdparty/sleipnir",
    )


def main():
    name = "sleipnir"
    url = "https://github.com/SleipnirGroup/Sleipnir"
    # main on 2024-12-07
    tag = "01206ab17d741f4c45a7faeb56b8a5442df1681c"

    sleipnir = Lib(name, url, tag, copy_upstream_src)
    sleipnir.main()


if __name__ == "__main__":
    main()
