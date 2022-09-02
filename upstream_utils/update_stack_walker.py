#!/usr/bin/env python3

import os
import shutil
import subprocess

from upstream_utils import (
    get_repo_root,
    clone_repo,
    comment_out_invalid_includes,
    walk_cwd_and_copy_if,
    git_am,
)


def crlf_to_lf(stackwalker_dir):
    for root, _, files in os.walk(stackwalker_dir):
        if ".git" in root:
            continue

        for fname in files:
            filename = os.path.join(root, fname)
            print(f"Converting CRLF -> LF for {filename}")
            with open(filename, "rb") as f:
                content = f.read()
                content = content.replace(b"\r\n", b"\n")

                with open(filename, "wb") as f:
                    f.write(content)

    cwd = os.getcwd()
    os.chdir(stackwalker_dir)
    subprocess.check_call(["git", "add", "-A"])
    subprocess.check_call(["git", "commit", "-m", "Fix line endings"])
    os.chdir(cwd)


def main():
    upstream_root = clone_repo(
        "https://github.com/JochenKalmbach/StackWalker",
        "42e7a6e056a9e7aca911a7e9e54e2e4f90bc2652",
        shallow=False,
    )
    wpilib_root = get_repo_root()
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    # Run CRLF -> LF before trying any patches
    crlf_to_lf(upstream_root)

    # Apply patches to upstream Git repo
    os.chdir(upstream_root)
    for f in [
        "0001-Apply-PR-35.patch",
        "0002-Remove-_M_IX86-checks.patch",
        "0003-Add-advapi-pragma.patch",
    ]:
        git_am(
            os.path.join(wpilib_root, "upstream_utils/stack_walker_patches", f),
            ignore_whitespace=True,
        )

    shutil.copy(
        os.path.join("Main", "StackWalker", "StackWalker.h"),
        os.path.join(wpiutil, "src/main/native/windows/StackWalker.h"),
    )

    shutil.copy(
        os.path.join("Main", "StackWalker", "StackWalker.cpp"),
        os.path.join(wpiutil, "src/main/native/windows/StackWalker.cpp"),
    )


if __name__ == "__main__":
    main()
