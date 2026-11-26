#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib


def copy_upstream_src(wpilib_root: Path):
    pocketfft = (
        wpilib_root / "tools/filterdesigner/src/main/native/thirdparty/pocketfft"
    )

    # Delete old install
    shutil.rmtree(pocketfft, ignore_errors=True)
    pocketfft.mkdir(parents=True)

    # Copy the single header into allwpilib
    shutil.copyfile("pocketfft_hdronly.h", pocketfft / "pocketfft_hdronly.h")


def main():
    name = "pocketfft"
    url = "https://gitlab.mpcdf.mpg.de/mtr/pocketfft.git"
    # cpp branch; verify with `./pocketfft.py clone` and update if needed
    tag = "32424d2067c2e8043dc646a4e49754b2b40cc549"

    pocketfft = Lib(name, url, tag, copy_upstream_src)
    pocketfft.main()


if __name__ == "__main__":
    main()
