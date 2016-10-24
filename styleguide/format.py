#!/usr/bin/env python3
"""This script invokes format.py in the wpilibsuite/styleguide repository.

Set the WPI_FORMAT environment variable to its location on disk before use. For
example:

WPI_FORMAT="$HOME/styleguide" ./format.py
"""

import os
import subprocess
import sys


def main():
    path = os.environ.get("WPI_FORMAT")
    if path == None:
        print("Error: WPI_FORMAT environment variable not set")
        sys.exit(1)

    try:
        # Run main format.py script
        args = ["python3", path + "/format.py"]
        args.extend(sys.argv[1:])
        proc = subprocess.Popen(args)
        sys.exit(proc.wait())
    except FileNotFoundError:
        # Run main format.py script on windows
        args = ["py", "-3", path + "/format.py"]
        args.extend(sys.argv[1:])
        proc = subprocess.Popen(args)
        sys.exit(proc.wait())


if __name__ == "__main__":
    main()
