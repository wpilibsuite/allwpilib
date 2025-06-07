#!/usr/bin/env python3

import os
import shutil
from pathlib import Path

from upstream_utils import Lib, walk_cwd_and_copy_if


def run_source_replacements(memory_files: list[Path]):
    for wpi_file in memory_files:
        with open(wpi_file) as f:
            content = f.read()

        # Fix #includes
        content = content.replace('include "', 'include "wpi/memory/')
        content = content.replace(
            "wpi/memory/free_list_utils.hpp", "free_list_utils.hpp"
        )

        with open(wpi_file, "w") as f:
            f.write(content)


def run_header_replacements(memory_files: list[Path]):
    for wpi_file in memory_files:
        if "detail" not in wpi_file.parts:
            continue
        with open(wpi_file) as f:
            content = f.read()

        # Fix #includes
        content = content.replace('include "config.hpp', 'include "../config.hpp')

        with open(wpi_file, "w") as f:
            f.write(content)


def run_global_replacements(memory_files: list[Path]):
    for wpi_file in memory_files:
        with open(wpi_file) as f:
            content = f.read()

        # Rename namespace from foonathan to wpi
        content = content.replace("namespace foonathan", "namespace wpi")
        content = content.replace("foonathan::", "wpi::")
        content = content.replace("FOONATHAN_", "WPI_")

        # Fix #includes
        content = content.replace('include "foonathan', 'include "wpi')

        with open(wpi_file, "w") as f:
            f.write(content)


def copy_upstream_src(wpilib_root: Path):
    upstream_root = Path(".").absolute()
    wpiutil = wpilib_root / "wpiutil"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/memory/src",
        "src/main/native/thirdparty/memory/include",
    ]:
        shutil.rmtree(wpiutil / d, ignore_errors=True)

    # Copy sources
    os.chdir(upstream_root / "src")
    src_files = walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".cpp") or f.endswith(".hpp"),
        wpiutil / "src/main/native/thirdparty/memory/src",
    )
    run_global_replacements(src_files)
    run_source_replacements(src_files)

    # Copy headers
    os.chdir(upstream_root / "include/foonathan")
    include_files = walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".hpp"),
        wpiutil / "src/main/native/thirdparty/memory/include/wpi",
    )
    os.chdir(upstream_root)
    run_global_replacements(include_files)
    run_header_replacements(include_files)

    # Copy config_impl.hpp
    shutil.copyfile(
        wpilib_root / "upstream_utils/memory_files/config_impl.hpp",
        wpiutil
        / "src/main/native/thirdparty/memory/include/wpi/memory/config_impl.hpp",
    )


def main():
    name = "memory"
    url = "https://github.com/foonathan/memory"
    tag = "v0.7-3"

    memory = Lib(name, url, tag, copy_upstream_src)
    memory.main()


if __name__ == "__main__":
    main()
