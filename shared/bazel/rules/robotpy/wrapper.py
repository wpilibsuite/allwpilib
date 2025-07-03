import importlib
import os
import sys

"""
This file will wrap various semiwrap.tools executables. In the event that it fails
it will provide more helpful debug information for bazel users
"""


def main():
    tool = sys.argv[1]
    args = sys.argv[2:]

    module = importlib.import_module(tool)
    tool_main = getattr(module, "main")

    sys.argv = [""] + args
    try:
        tool_main()
    except:
        print("-------------------------------------")
        print("Failed to run wrapped tool.")
        print(f"CWD: {os.getcwd()}")
        print(f"Tool: {tool}, Args:")
        for a in args:
            print("  ", a)
        print("-------------------------------------")
        raise


if __name__ == "__main__":
    main()
