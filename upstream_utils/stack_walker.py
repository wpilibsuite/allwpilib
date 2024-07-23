#!/usr/bin/env python3

import os
import shutil
import subprocess

from upstream_utils import Lib


def crlf_to_lf():
    for root, _, files in os.walk("."):
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

    subprocess.check_call(["git", "add", "-A"])
    subprocess.check_call(["git", "commit", "-m", "Fix line endings"])


def copy_upstream_src(wpilib_root):
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    shutil.copy(
        os.path.join("Main", "StackWalker", "StackWalker.h"),
        os.path.join(wpiutil, "src/main/native/windows/StackWalker.h"),
    )

    shutil.copy(
        os.path.join("Main", "StackWalker", "StackWalker.cpp"),
        os.path.join(wpiutil, "src/main/native/windows/StackWalker.cpp"),
    )


def main():
    name = "stack_walker"
    url = "https://github.com/JochenKalmbach/StackWalker"
    tag = "5b0df7a4db8896f6b6dc45d36e383c52577e3c6b"

    patch_options = {
        "ignore_whitespace": True,
    }

    stack_walker = Lib(
        name,
        url,
        tag,
        copy_upstream_src,
        patch_options,
        pre_patch_hook=crlf_to_lf,
        pre_patch_commits=1,
    )
    stack_walker.main()


if __name__ == "__main__":
    main()
