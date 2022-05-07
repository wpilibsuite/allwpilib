#!/usr/bin/env python3

import os
import shutil
import re
import requests
import tempfile
import urllib
import subprocess

from upstream_utils import setup_upstream_repo, comment_out_invalid_includes, walk_cwd_and_copy_if, am_patches, walk_if, copy_to


def main():
    root, repo = setup_upstream_repo(
        "https://github.com/JochenKalmbach/StackWalker",
        "42e7a6e056a9e7aca911a7e9e54e2e4f90bc2652")
    wpiutil = os.path.join(root, "wpiutil")

    pr35_url = "http://patch-diff.githubusercontent.com/raw/JochenKalmbach/StackWalker/pull/35.patch"
    pr35_patch_download_path = os.path.join(tempfile.gettempdir(),
                                            "stackwalker.patch")

    response = urllib.request.urlopen(pr35_url)

    with open(pr35_patch_download_path, 'wb') as f:
        f.write(response.read())

    am_patches(repo, [pr35_patch_download_path])

    shutil.copy(os.path.join("Main", "StackWalker", "StackWalker.h"),
                os.path.join(wpiutil, "src/main/native/windows/StackWalker.h"))

    shutil.copy(
        os.path.join("Main", "StackWalker", "StackWalker.cpp"),
        os.path.join(wpiutil, "src/main/native/windows/StackWalker.cpp"))


if __name__ == "__main__":
    main()
