#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib, has_prefix, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    mrcal = wpilib_root / "mrcal"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/src",
        "src/main/native/thirdparty/include",
    ]:
        shutil.rmtree(mrcal / d, ignore_errors=True)

    files = walk_cwd_and_copy_if(
        lambda dp, f: (
            f.endswith((".h", ".hh"))
            and f != "heap.h"
            and f != "stereo-matching-libelas.h"
            and f != "python-wrapping-utilities.h"
            and f != "python-cameramodel-converter.h"
            and not has_prefix(dp, Path("test"))
        ),
        mrcal / "src/main/native/thirdparty/include",
    )
    files = files + walk_cwd_and_copy_if(
        lambda dp, f: (
            f.endswith((".c", ".cc", ".cpp", ".pl"))
            and f
            not in [
                "heap.cc",
                "mrcal-pywrap.c",
                "image.c",
                "python-cameramodel-converter.c",
                "stereo.c",
                "stereo-matching-libelas.cc",
                "uncertainty.c",
                "traverse-sensor-links.c",
            ]
            and not has_prefix(dp, Path("doc"))
            and not has_prefix(dp, Path("test"))
        ),
        mrcal / "src/main/native/thirdparty/src",
    )

    for f in files:
        with open(f) as file:
            content = file.read()
        content = content.replace("#warning", "// #warning")
        with open(f, "w") as file:
            file.write(content)
        if f.suffix == ".pl":
            f.chmod(0o755)


def main():
    name = "mrcal"
    url = "https://github.com/dkogan/mrcal"
    # master on 2026-06-30
    tag = "c47576ea206f3cbd1ff86ee748c1b00bdd93361b"

    mrcal = Lib(name, url, tag, copy_upstream_src)
    mrcal.main()


if __name__ == "__main__":
    main()
