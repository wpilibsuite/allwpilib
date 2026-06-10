#!/usr/bin/env python3

import shutil
import subprocess
from pathlib import Path

from upstream_utils import Lib


def copy_upstream_src(wpilib_root: Path):
    wpinet = wpilib_root / "wpinet"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/llhttp/src",
        "src/main/native/thirdparty/llhttp/include",
    ]:
        shutil.rmtree(wpinet / d, ignore_errors=True)
        (wpinet / d).mkdir(parents=True, exist_ok=True)

    subprocess.check_call("npm i", shell=True)
    subprocess.check_call("npm run build", shell=True)
    # Copy files into allwpilib
    shutil.copyfile(
        "build/llhttp.h",
        wpinet / "src/main/native/thirdparty/llhttp/include" / "llhttp.h",
    )
    shutil.copyfile(
        "build/c/llhttp.c",
        wpinet / "src/main/native/thirdparty/llhttp/src/llhttp.c",
    )
    shutil.copyfile(
        "src/native/api.c", wpinet / "src/main/native/thirdparty/llhttp/src/api.c"
    )
    shutil.copyfile(
        "src/native/http.c", wpinet / "src/main/native/thirdparty/llhttp/src/http.c"
    )


def main():
    name = "llhttp"
    url = "https://github.com/nodejs/llhttp"
    tag = "v9.4.1"

    llhttp = Lib(name, url, tag, copy_upstream_src)
    llhttp.main()


if __name__ == "__main__":
    main()
