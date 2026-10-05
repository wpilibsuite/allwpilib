#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    mrcal = wpilib_root / "mrcal"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/libdogleg/src",
        "src/main/native/thirdparty/libdogleg/include",
    ]:
        shutil.rmtree(mrcal / d, ignore_errors=True)

    walk_cwd_and_copy_if(
        lambda dp, f: f == "dogleg.h",
        mrcal / "src/main/native/thirdparty/libdogleg/include",
    )

    files = walk_cwd_and_copy_if(
        lambda dp, f: f == "dogleg.c",
        mrcal / "src/main/native/thirdparty/libdogleg/src",
    )
    for f in files:
        with open(f) as file:
            content = file.read()
        content = content.replace("#warning", "// #warning")
        content = content.replace("__attribute__((unused))", "")
        with open(f, "w") as file:
            file.write(content)


def main():
    name = "libdogleg"
    url = "https://github.com/dkogan/libdogleg"
    # master on 2026-05-25
    tag = "75977739cea87f22c94f8391b24c36a5a1ba8a2d"

    libdogleg = Lib(name, url, tag, copy_upstream_src)
    libdogleg.main()


if __name__ == "__main__":
    main()
