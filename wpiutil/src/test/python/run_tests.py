#!/usr/bin/env python3

import os
from os.path import abspath, dirname
import sys
import subprocess

if __name__ == "__main__":
    root = abspath(dirname(__file__))
    os.chdir(root)

    subprocess.check_call(
        [
            sys.executable,
            "-m",
            "pip",
            "--disable-pip-version-check",
            "install",
            "-v",
            "--force-reinstall",
            "--no-build-isolation",
            "./cpp",
        ],
    )

    subprocess.check_call([sys.executable, "-m", "pytest"])
