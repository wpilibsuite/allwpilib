#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib, copy_to, has_prefix, walk_cwd_and_copy_if

nanopb_sources = [
    Path("pb_encode.c"),
    Path("pb_decode.c"),
    Path("pb_common.c"),
]

nanopb_headers = [
    Path("pb.h"),
    Path("pb_encode.h"),
    Path("pb_decode.h"),
    Path("pb_common.h"),
]

nanopb_generator = [
    Path("pb.h"),
    Path("pb_encode.h"),
    Path("pb_decode.h"),
    Path("pb_common.h"),
]


def copy_upstream_src(wpilib_root: Path):
    wpiutil = wpilib_root / "wpiutil"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/nanopb/src",
        "src/main/native/thirdparty/nanopb/include",
        "src/main/native/thirdparty/nanopb/generator",
    ]:
        shutil.rmtree(wpiutil / d, ignore_errors=True)

    # Copy nanopb source files into allwpilib
    copy_to(
        nanopb_sources,
        wpiutil / "src/main/native/thirdparty/nanopb/src",
        rename_c_to_cpp=True,
    )

    # Copy nanopb header files into allwpilib
    copy_to(
        nanopb_headers,
        wpiutil / "src/main/native/thirdparty/nanopb/include",
    )

    generator_files = walk_cwd_and_copy_if(
        lambda dp, f: has_prefix(dp, Path("generator")),
        wpiutil / "src/main/native/thirdparty/nanopb",
    )


def main():
    name = "nanopb"
    url = "https://github.com/nanopb/nanopb"
    tag = "0.4.9"

    nanopb = Lib(name, url, tag, copy_upstream_src)
    nanopb.main()


if __name__ == "__main__":
    main()
