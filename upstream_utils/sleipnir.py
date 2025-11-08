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
        and f not in [".styleguide", ".styleguide-license"],
        wpimath / "src/main/native/thirdparty/sleipnir",
    )

    # Write shim for wpi::SmallVector
    (wpimath / "src/main/native/thirdparty/sleipnir/include/gch").mkdir()
    with open(
        wpimath / "src/main/native/thirdparty/sleipnir/include/gch/small_vector.hpp",
        "w",
    ) as f:
        f.write(
            """// Copyright (c) Sleipnir contributors

#pragma once

#include <wpi/util/SmallVector.hpp>

namespace gch {

template <typename T>
using small_vector = wpi::util::SmallVector<T>;

}  // namespace gch
"""
        )


def main():
    name = "sleipnir"
    url = "https://github.com/SleipnirGroup/Sleipnir"
    # main on 2025-09-19
    tag = "7f89d5547702a09e3617bc31fe5bafe6add04fab"

    sleipnir = Lib(name, url, tag, copy_upstream_src)
    sleipnir.main()


if __name__ == "__main__":
    main()
