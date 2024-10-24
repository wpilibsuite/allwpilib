#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root):
    wpical = os.path.join(wpilib_root, "wpical")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/mrcal_java/src",
        "src/main/native/thirdparty/mrcal_java/include",
    ]:
        shutil.rmtree(os.path.join(wpical, d), ignore_errors=True)

    os.chdir("src")
    walk_cwd_and_copy_if(
        lambda dp, f: f.endswith("mrcal_wrapper.h"),
        os.path.join(wpical, "src/main/native/thirdparty/mrcal_java/include"),
    )
    walk_cwd_and_copy_if(
        lambda dp, f: f.endswith("mrcal_wrapper.cpp"),
        os.path.join(wpical, "src/main/native/thirdparty/mrcal_java/src"),
    )


def main():
    name = "mrcal-java"
    url = "https://github.com/PhotonVision/mrcal-java"
    tag = "d05b8ff0df95eafaf4321b5242882b28aaf23061"

    mrcal_java = Lib(name, url, tag, copy_upstream_src)
    mrcal_java.main()


if __name__ == "__main__":
    main()
