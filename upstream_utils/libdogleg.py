#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root):
    wpical = os.path.join(wpilib_root, "wpical")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/libdogleg/src",
        "src/main/native/thirdparty/libdogleg/include",
    ]:
        shutil.rmtree(os.path.join(wpical, d), ignore_errors=True)

    files = walk_cwd_and_copy_if(
        lambda dp, f: f.endswith("dogleg.h"),
        os.path.join(wpical, "src/main/native/thirdparty/libdogleg/include"),
    )
    for f in files:
        with open(f) as file:
            content = file.read()
        content = content.replace(
            "#include <cholmod.h>", "#include <suitesparse/cholmod.h>"
        )
        with open(f, "w") as file:
            file.write(content)

    files = walk_cwd_and_copy_if(
        lambda dp, f: f.endswith("dogleg.cpp"),
        os.path.join(wpical, "src/main/native/thirdparty/libdogleg/src"),
    )
    for f in files:
        with open(f) as file:
            content = file.read()
        content = content.replace("#warning", "// #warning")
        content = content.replace("__attribute__((unused))", "")
        content = content.replace(
            "#include <cholmod_function.h>", "#include <suitesparse/cholmod_function.h>"
        )
        with open(f, "w") as file:
            file.write(content)


def main():
    name = "libdogleg"
    url = "https://github.com/dkogan/libdogleg"
    tag = "c971ea43088d286a3683c1039b9a85f761f7df15"

    libdogleg = Lib(name, url, tag, copy_upstream_src)
    libdogleg.main()


if __name__ == "__main__":
    main()
