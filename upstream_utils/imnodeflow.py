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

    # Copy implementation files. Upstream's include/ImNodeFlow.h reaches into
    # ../src/ for inline templates and helper headers, so the include/ + src/
    # split must be preserved verbatim.
    walk_cwd_and_copy_if(
        lambda dp, f: dp == Path("src")
        and (f.endswith(".cpp") or f.endswith(".inl") or f.endswith(".h")),
        imnodeflow,
    )


def main():
    name = "imnodeflow"
    url = "https://github.com/Fattorino/ImNodeFlow.git"
    tag = "v1.2.2"

    imnodeflow = Lib(name, url, tag, copy_upstream_src)
    imnodeflow.main()


if __name__ == "__main__":
    main()
