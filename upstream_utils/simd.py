#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, copy_to


def copy_upstream_src(wpilib_root):
    simd = os.path.join(wpilib_root, "thirdparty/simd")

    # Delete old install
    for d in [
        "src/main/native/simd/cpp",
        "src/main/native/simd/include",
    ]:
        shutil.rmtree(os.path.join(simd, d), ignore_errors=True)

    # Copy Simd source files into allwpilib
    src_files = [
        os.path.join(dp, f)
        for dp, dn, fn in os.walk("src/Simd")
        for f in fn
        if f.endswith(".cpp")
    ]
    src_files = copy_to(src_files, os.path.join(simd, "src/main/native/simd/cpp"))

    # Copy Simd header files into allwpilib
    include_files = [
        os.path.join(dp, f)
        for dp, dn, fn in os.walk("src/Simd")
        for f in fn
        if f.endswith(".hpp") or f.endswith(".h")
    ]
    include_files = copy_to(
        include_files, os.path.join(simd, "src/main/native/simd/include")
    )


def main():
    name = "simd"
    url = "https://github.com/ermig1979/Simd.git"
    # main on 2025-05-05
    tag = "30d1bdecf215926b2f258c9f3290d711436541c7"

    simd = Lib(name, url, tag, copy_upstream_src)
    simd.main()


if __name__ == "__main__":
    main()
