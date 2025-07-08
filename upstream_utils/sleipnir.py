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
        or f == ".clang-format"
        or f == ".clang-tidy"
        or f == ".styleguide"
        or f == ".styleguide-license",
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

#include <wpi/SmallVector.h>

namespace gch {

template <typename T>
using small_vector = wpi::SmallVector<T>;

}  // namespace gch
"""
        )


def main():
    name = "sleipnir"
    url = "https://github.com/SleipnirGroup/Sleipnir"
    # main on 2025-05-18
    tag = "2cc18ff6d25ee0a9bd0f9993a0a41a61a28bda3e"

    sleipnir = Lib(name, url, tag, copy_upstream_src)
    sleipnir.main()


if __name__ == "__main__":
    main()
