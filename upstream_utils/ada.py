#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import (
    Lib,
    comment_out_invalid_includes,
    has_prefix,
    walk_cwd_and_copy_if,
)


def copy_upstream_src(wpilib_root: Path):
    wpinet = wpilib_root / "wpinet"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/ada/src",
        "src/main/native/thirdparty/ada/include",
    ]:
        shutil.rmtree(wpinet / d, ignore_errors=True)
        (wpinet / d).mkdir(parents=True, exist_ok=True)

    # Copy files into allwpilib
    src_files = walk_cwd_and_copy_if(
        lambda dp, f: (
            has_prefix(dp, Path("src"))
            and f.endswith(".cpp")
            and f != "ada_c.cpp"
            and not f.startswith("url_pattern")
        ),
        wpinet / "src/main/native/thirdparty/ada",
    )

    # Copy header files into allwpilib
    header_files = walk_cwd_and_copy_if(
        lambda dp, f: (
            has_prefix(dp, Path("include"))
            and f != "ada_c.h"
            and f != "expected.h"
            and not f.startswith("url_pattern")
        ),
        wpinet / "src/main/native/thirdparty/ada",
    )
    for f in header_files + src_files:
        comment_out_invalid_includes(
            f,
            [wpinet / "src/main/native/thirdparty/ada/include"],
        )


def main():
    name = "ada"
    url = "https://github.com/ada-url/ada"
    tag = "v3.4.4"

    ada = Lib(name, url, tag, copy_upstream_src)
    ada.main()


if __name__ == "__main__":
    main()
