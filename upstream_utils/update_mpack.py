#!/usr/bin/env python3

import os
import shutil
import subprocess

from upstream_utils import (
    get_repo_root,
    clone_repo,
    walk_cwd_and_copy_if,
    git_am,
)


def main():
    upstream_root = clone_repo("https://github.com/ludocode/mpack", "v1.1")
    wpilib_root = get_repo_root()
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/mpack/src",
        "src/main/native/thirdparty/mpack/include",
    ]:
        shutil.rmtree(os.path.join(wpiutil, d), ignore_errors=True)

    # Apply patches to upstream Git repo
    os.chdir(upstream_root)

    for f in [
        "0001-Don-t-emit-inline-defs.patch",
        "0002-Update-amalgamation-script.patch",
        "0003-Use-namespace-for-C.patch",
    ]:
        git_am(
            os.path.join(wpilib_root, "upstream_utils/mpack_patches", f),
        )

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


if __name__ == "__main__":
    main()
