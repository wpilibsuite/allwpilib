#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import Lib, copy_to, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    imnodeflow = wpilib_root / "thirdparty/imgui_suite/imnodeflow"

    # Delete old install
    for d in ["include", "src"]:
        shutil.rmtree(imnodeflow / d, ignore_errors=True)
    imnodeflow.mkdir(parents=True, exist_ok=True)

    # Copy public header (single file)
    copy_to([Path("include/ImNodeFlow.h")], imnodeflow)

    # Copy implementation files
    walk_cwd_and_copy_if(
        lambda dp, f: dp == Path("src") and f.endswith((".cpp", ".inl", ".h")),
        imnodeflow,
    )


def main():
    name = "imnodeflow"
    url = "https://github.com/Fattorino/ImNodeFlow.git"
    tag = "e4cb0989c302e0ae324ff03ed1b3f0d2851ee630"

    imnodeflow = Lib(name, url, tag, copy_upstream_src)
    imnodeflow.main()


if __name__ == "__main__":
    main()
