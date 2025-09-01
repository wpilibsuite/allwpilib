#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib, has_prefix, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    mrcal = wpilib_root / "thirdparty/mrcal"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/src",
        "src/main/native/thirdparty/include",
    ]:
        shutil.rmtree(mrcal / d, ignore_errors=True)

    files = walk_cwd_and_copy_if(
        lambda dp, f: (f.endswith(".h") or f.endswith(".hh"))
        and not f == "heap.h"
        and not f == "stereo-matching-libelas.h"
        and not has_prefix(dp, Path("test")),
        mrcal / "src/main/native/thirdparty/include",
    )
    files = files + walk_cwd_and_copy_if(
        lambda dp, f: (
            f.endswith(".c")
            or f.endswith(".cc")
            or f.endswith(".cpp")
            or f.endswith(".pl")
        )
        and not f == "heap.cc"
        and not f == "mrcal-pywrap.c"
        and not f == "image.c"
        and not f == "stereo.c"
        and not f == "stereo-matching-libelas.cc"
        and not f == "uncertainty.c"
        and not f == "traverse-sensor-links.c"
        and not has_prefix(dp, Path("doc"))
        and not has_prefix(dp, Path("test")),
        mrcal / "src/main/native/thirdparty/src",
    )

    for f in files:
        with open(f) as file:
            content = file.read()
        content = content.replace("#warning", "// #warning")
        content = content.replace('__attribute__ ((visibility ("hidden")))', "")
        content = content.replace("__attribute__((unused))", "")
        with open(f, "w") as file:
            file.write(content)


def main():
    name = "mrcal"
    url = "https://github.com/dkogan/mrcal"
    # master on 2024-11-29
    tag = "662a539d3cbba4948c31d06a780569173b3fb6e6"

    mrcal = Lib(name, url, tag, copy_upstream_src)
    mrcal.main()


if __name__ == "__main__":
    main()
