#!/usr/bin/env python

import os
import shutil
from pathlib import Path

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    wpical = wpilib_root / "tools/wpical"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/mrcal_java/src",
        "src/main/native/thirdparty/mrcal_java/include",
    ]:
        shutil.rmtree(wpical / d, ignore_errors=True)

    os.chdir("src")
    walk_cwd_and_copy_if(
        lambda dp, f: f == "mrcal_wrapper.h",
        wpical / "src/main/native/thirdparty/mrcal_java/include",
    )

    walk_cwd_and_copy_if(
        lambda dp, f: f == "mrcal_wrapper.cpp",
        wpical / "src/main/native/thirdparty/mrcal_java/src",
    )


def main():
    name = "mrcal_java"
    url = "https://github.com/PhotonVision/mrcal-java"
    # main on 2026-07-31
    tag = "e9fd96af6ae886fcba3b48628d11629a94a305f6"

    mrcal_java = Lib(name, url, tag, copy_upstream_src)
    mrcal_java.main()


if __name__ == "__main__":
    main()
