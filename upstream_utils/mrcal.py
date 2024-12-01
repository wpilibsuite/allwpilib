#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root):
    wpical = os.path.join(wpilib_root, "wpical")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/mrcal/src",
        "src/main/native/thirdparty/mrcal/include",
    ]:
        shutil.rmtree(os.path.join(wpical, d), ignore_errors=True)

    files = walk_cwd_and_copy_if(
        lambda dp, f: (f.endswith(".h") or f.endswith(".hh"))
        and not f.endswith("heap.h")
        and not f.endswith("stereo-matching-libelas.h")
        and not dp.startswith(os.path.join(".", "test")),
        os.path.join(wpical, "src/main/native/thirdparty/mrcal/include"),
    )
    files = files + walk_cwd_and_copy_if(
        lambda dp, f: (
            f.endswith(".c")
            or f.endswith(".cc")
            or f.endswith(".cpp")
            or f.endswith(".pl")
        )
        and not f.endswith("heap.cc")
        and not f.endswith("mrcal-pywrap.c")
        and not f.endswith("image.c")
        and not f.endswith("stereo.c")
        and not f.endswith("stereo-matching-libelas.cc")
        and not f.endswith("uncertainty.c")
        and not f.endswith("traverse-sensor-links.c")
        and not dp.startswith(os.path.join(".", "doc"))
        and not dp.startswith(os.path.join(".", "test")),
        os.path.join(wpical, "src/main/native/thirdparty/mrcal/src"),
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
    tag = "662a539d3cbba4948c31d06a780569173b3fb6e6"

    mrcal = Lib(name, url, tag, copy_upstream_src)
    mrcal.main()


if __name__ == "__main__":
    main()
