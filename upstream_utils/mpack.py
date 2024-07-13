#!/usr/bin/env python3

import os
import shutil
import subprocess

from upstream_utils import (
    get_repo_root,
    clone_repo,
    walk_cwd_and_copy_if,
    git_am,
    Lib,
)


def copy_upstream_src(wpilib_root):
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/mpack/src",
        "src/main/native/thirdparty/mpack/include",
    ]:
        shutil.rmtree(os.path.join(wpiutil, d), ignore_errors=True)

    # Run the amalgmation script
    subprocess.check_call(["bash", "tools/amalgamate.sh"])

    # Copy the files
    amalgamation_source_dir = os.path.join(
        ".", ".build", "amalgamation", "src", "mpack"
    )
    os.chdir(amalgamation_source_dir)

    walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".h"),
        os.path.join(wpiutil, "src/main/native/thirdparty/mpack/include/wpi"),
    )
    walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".c"),
        os.path.join(wpiutil, "src/main/native/thirdparty/mpack/src"),
    )


def main():
    name = "mpack"
    url = "https://github.com/ludocode/mpack"
    tag = "v1.1.1"

    patch_list = [
        "0001-Don-t-emit-inline-defs.patch",
        "0002-Update-amalgamation-script.patch",
        "0003-Use-namespace-for-C.patch",
        "0004-Group-doxygen-into-MPack-module.patch",
    ]

    mpack = Lib(name, url, tag, [], copy_upstream_src)
    mpack.main()


if __name__ == "__main__":
    main()
