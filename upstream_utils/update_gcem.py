#!/usr/bin/env python3

import os
import shutil

from upstream_utils import (
    get_repo_root,
    clone_repo,
    comment_out_invalid_includes,
    walk_cwd_and_copy_if,
    git_am,
)


def main():
    upstream_root = clone_repo("https://github.com/kthohr/gcem.git", "v1.16.0")
    wpilib_root = get_repo_root()
    wpimath = os.path.join(wpilib_root, "wpimath")

    # Apply patches to upstream Git repo
    os.chdir(upstream_root)
    for f in []:
        git_am(os.path.join(wpilib_root, "upstream_utils/gcem_patches", f))

    # Delete old install
    for d in [
        "src/main/native/thirdparty/gcem/include",
    ]:
        shutil.rmtree(os.path.join(wpimath, d), ignore_errors=True)

    # Copy gcem include files into allwpilib
    include_files = walk_cwd_and_copy_if(
        lambda dp, f: dp.endswith("include")
        or dp.endswith("gcem_incl")
        or dp.endswith("quadrature"),
        os.path.join(wpimath, "src/main/native/thirdparty/gcem"),
    )

    for f in include_files:
        comment_out_invalid_includes(
            f, [os.path.join(wpimath, "src/main/native/thirdparty/gcem/include")]
        )


if __name__ == "__main__":
    main()
