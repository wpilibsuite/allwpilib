#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, copy_to


def copy_upstream_src(wpilib_root):
    wpimath = os.path.join(wpilib_root, "wpimath")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/sleipnir/src",
        "src/main/native/thirdparty/sleipnir/include",
    ]:
        shutil.rmtree(os.path.join(wpimath, d), ignore_errors=True)

    # Copy Sleipnir source files into allwpilib
    src_files = [os.path.join(dp, f) for dp, dn, fn in os.walk("src") for f in fn]
    src_files = copy_to(
        src_files, os.path.join(wpimath, "src/main/native/thirdparty/sleipnir")
    )

    # Copy Sleipnir header files into allwpilib
    include_files = [
        os.path.join(dp, f) for dp, dn, fn in os.walk("include") for f in fn
    ]
    include_files = copy_to(
        include_files, os.path.join(wpimath, "src/main/native/thirdparty/sleipnir")
    )

    for filename in [
        ".clang-format",
        ".clang-tidy",
        ".styleguide",
        ".styleguide-license",
    ]:
        shutil.copyfile(
            filename,
            os.path.join(wpimath, "src/main/native/thirdparty/sleipnir", filename),
        )

    # Write shim for wpi::SmallVector
    try:
        os.mkdir(
            os.path.join(wpimath, "src/main/native/thirdparty/sleipnir/include/gch")
        )
    except:
        pass
    with open(
        os.path.join(
            wpimath,
            "src/main/native/thirdparty/sleipnir/include/gch/small_vector.hpp",
        ),
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
