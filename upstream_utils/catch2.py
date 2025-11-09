#!/usr/bin/env python3

import os
import shutil
from pathlib import Path

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root: Path):
    upstream_root = Path(".").absolute()
    catch2 = wpilib_root / "thirdparty/catch2"

    # Delete old install
    for d in ["src/main/native", "extras"]:
        shutil.rmtree(catch2 / d, ignore_errors=True)

    # Copy Catch2 source files into allwpilib
    walk_cwd_and_copy_if(
        lambda dp, f: f == "Catch.cmake" or f == "CatchAddTests.cmake",
        catch2,
    )
    os.chdir(upstream_root / "src")
    walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".cpp")
        and "catch2" in dp.parts
        and f != "catch_main.cpp",
        catch2 / "src/main/native/cpp",
    )
    walk_cwd_and_copy_if(
        lambda dp, f: f.endswith(".hpp")
        and ("third_party" in dp.parts or "catch2" in dp.parts),
        catch2 / "src/main/native/include",
    )

    # Configure the user config as Catch2's CMake system would've, which is to not define anything
    with open(upstream_root / "src/catch2/catch_user_config.hpp.in") as f:
        content = f.read()
        content = content.replace("#cmakedefine", "// #undef")
        content = content.replace("@CATCH_CONFIG_DEFAULT_REPORTER@", "console")
        content = content.replace("@CATCH_CONFIG_CONSOLE_WIDTH@", "80")

    with open(
        catch2 / "src/main/native/include/catch2/catch_user_config.hpp", "w"
    ) as f:
        f.write(content)


def main():
    name = "catch2"
    url = "https://github.com/catchorg/Catch2.git"
    tag = "v3.10.0"

    patch_options = {
        "ignore_whitespace": True,
    }

    catch2 = Lib(name, url, tag, copy_upstream_src, patch_options)
    catch2.main()


if __name__ == "__main__":
    main()
