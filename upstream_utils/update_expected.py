#!/usr/bin/env python3

import os
import re
import shutil

from upstream_utils import (
    get_repo_root,
    clone_repo,
    comment_out_invalid_includes,
    walk_cwd_and_copy_if,
    git_am,
)


def main():
    upstream_root = clone_repo(
        "https://github.com/TartanLlama/expected",
        # master on 2024-01-25
        "3f0ca7b19253129700a073abfa6d8638d9f7c80c",
        shallow=False,
    )
    wpilib_root = get_repo_root()
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    # Copy expected header into allwpilib
    dest_filename = os.path.join(
        wpiutil, "src/main/native/thirdparty/expected/include/wpi/expected"
    )
    shutil.copyfile(
        os.path.join(upstream_root, "include/tl/expected.hpp"), dest_filename
    )

    # Rename namespace from tl to wpi
    with open(dest_filename) as f:
        content = f.read()
    content = content.replace("namespace tl", "namespace wpi")
    content = content.replace("tl::", "wpi::")
    content = content.replace("TL_", "WPI_")
    with open(dest_filename, "w") as f:
        f.write(content)


if __name__ == "__main__":
    main()
