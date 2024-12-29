#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, copy_to, walk_if

nanopb_sources = set(
    [
        "pb_encode.c",
        "pb_decode.c",
        "pb_common.c",
    ]
)

nanopb_headers = set(
    [
        "pb.h",
        "pb_encode.h",
        "pb_decode.h",
        "pb_common.h",
    ]
)

nanopb_generator = set(
    [
        "pb.h",
        "pb_encode.h",
        "pb_decode.h",
        "pb_common.h",
    ]
)


def copy_upstream_src(wpilib_root):
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/nanopb/src",
        "src/main/native/thirdparty/nanopb/include",
        "src/main/native/thirdparty/nanopb/generator",
    ]:
        shutil.rmtree(os.path.join(wpiutil, d), ignore_errors=True)

    # Copy nanopb source files into allwpilib
    copy_to(
        nanopb_sources,
        os.path.join(wpiutil, "src/main/native/thirdparty/nanopb/src"),
        rename_c_to_cpp=True,
    )

    # Copy nanopb header files into allwpilib
    copy_to(
        nanopb_headers,
        os.path.join(wpiutil, "src/main/native/thirdparty/nanopb/include"),
    )

    generator_files = walk_if("generator", lambda dp, f: True)
    copy_to(
        generator_files,
        os.path.join(wpiutil, "src/main/native/thirdparty/nanopb"),
    )


def main():
    name = "nanopb"
    url = "https://github.com/nanopb/nanopb"
    tag = "0.4.9"

    nanopb = Lib(name, url, tag, copy_upstream_src)
    nanopb.main()


if __name__ == "__main__":
    main()
