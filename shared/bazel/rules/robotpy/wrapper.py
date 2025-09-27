import importlib
import os
import pathlib
import sys

from shared.bazel.rules.robotpy.hack_pkgcfgs import hack_pkgconfig

"""
This file will wrap various semiwrap.tools executables. In the event that it fails
it will provide more helpful debug information for bazel users.

It can also "hack" the PKG_CONFIG_PATH environment variable. This allows us to use
generated package config files without having to install the libraries which decreases
build dependencies and increases the amount of parallelization that can happen during
the build.
"""


def main():
    tool = sys.argv[1]

    if "--pkgcfgs" in sys.argv[2:]:
        pkgcfg_index = sys.argv.index("--pkgcfgs")
        args = sys.argv[2:pkgcfg_index]
        pkgcfgs = [pathlib.Path(x) for x in sys.argv[pkgcfg_index + 1 :]]
    else:
        args = sys.argv[2:]
        pkgcfgs = []

    hack_pkgconfig(pkgcfgs)

    module = importlib.import_module(tool)
    tool_main = getattr(module, "main")

    sys.argv = [""] + args
    try:
        tool_main()
    except SystemExit as e:
        if e.code != 0:
            raise Exception(
                "sys.exit() explicitly called with a non-zero error code", e
            )
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
