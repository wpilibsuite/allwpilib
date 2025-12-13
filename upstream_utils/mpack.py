#!/usr/bin/env python3

import os
import shutil
import subprocess
from pathlib import Path

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    wpiutil = wpilib_root / "wpiutil"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/mpack/src",
        "src/main/native/thirdparty/mpack/include",
    ]:
        shutil.rmtree(wpiutil / d, ignore_errors=True)

    # Run the amalgmation script
    subprocess.check_call(["bash", "tools/amalgamate.sh"])

    # Copy the files
    os.chdir(Path(".build/amalgamation/src/mpack"))

    walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".h"),
        wpiutil / "src/main/native/thirdparty/mpack/include/wpi/util",
    )
    walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".c"),
        wpiutil / "src/main/native/thirdparty/mpack/src",
        rename_c_to_cpp=True,
    )


def main():
    name = "mpack"
    url = "https://github.com/ludocode/mpack"
    tag = "v1.1.1"

    mpack = Lib(name, url, tag, copy_upstream_src)
    mpack.main()


if __name__ == "__main__":
    main()
