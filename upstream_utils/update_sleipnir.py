#!/usr/bin/env python3

import os
import shutil

from upstream_utils import (
    get_repo_root,
    clone_repo,
    copy_to,
    walk_cwd_and_copy_if,
    git_am,
)


def main():
    upstream_root = clone_repo(
        "https://github.com/SleipnirGroup/Sleipnir",
        # main on 2024-06-05
        "57005c8b740ab163d3f7ef7e0c7c7c8774f4f925",
        shallow=False,
    )
    wpilib_root = get_repo_root()
    wpimath = os.path.join(wpilib_root, "wpimath")

    # Apply patches to upstream Git repo
    os.chdir(upstream_root)
    for f in [
        "0001-Remove-using-enum-declarations.patch",
        "0002-Add-implicit-typename.patch",
        "0003-Use-fmtlib.patch",
        "0004-Remove-unsupported-constexpr.patch",
    ]:
        git_am(os.path.join(wpilib_root, "upstream_utils/sleipnir_patches", f))

    # Delete old install
    for d in [
        "src/main/native/thirdparty/sleipnir/src",
        "src/main/native/thirdparty/sleipnir/include",
    ]:
        shutil.rmtree(os.path.join(wpimath, d), ignore_errors=True)

    # Copy Sleipnir source files into allwpilib
    os.chdir(upstream_root)
    src_files = [os.path.join(dp, f) for dp, dn, fn in os.walk("src") for f in fn]
    src_files = copy_to(
        src_files, os.path.join(wpimath, "src/main/native/thirdparty/sleipnir")
    )

    # Copy Sleipnir header files into allwpilib
    include_files = [
        os.path.join(dp, f) for dp, dn, fn in os.walk("include") for f in fn
    ]
    include_files = copy_to(
        include_files, os.path.join(wpimath, "src/main/native/thirdparty/sleipnir")
    )

    for filename in [
        ".clang-format",
        ".clang-tidy",
        ".styleguide",
        ".styleguide-license",
    ]:
        shutil.copyfile(
            os.path.join(upstream_root, filename),
            os.path.join(wpimath, "src/main/native/thirdparty/sleipnir", filename),
        )


if __name__ == "__main__":
    main()
