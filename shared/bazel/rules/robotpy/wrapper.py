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


README_LINK = "https://github.com/wpilibsuite/allwpilib/blob/2027/README-RobotPy.md"


def print_failure_message(tool, args):

    extra_error_message = None

    # Some tools are more likely to fail, and can give a better error description to help the user debug their problem.
    if tool == "semiwrap.tool":
        if args[0] == "scan-headers":
            extra_error_message = f"""
#########################################################################################
The scan header test has failed. This likely means that you have removed header
files that are used by the project or have added new headers that aren't tracked
(or explicitly ignored) by the project. The text printed up above this message
should tell you want needs to be added / removed from this project's pyproject.toml file.

Please see this readme for more information:
{README_LINK}#1-scan-headers
#########################################################################################
"""

    elif tool == "semiwrap.cmd.header2dat":
        extra_error_message = f"""
#########################################################################################
The 'header to dat file' step has failed. The most probable cause for this is that you
have yaml files that don't exist or are out of date called out in this project's
pyproject.toml file, and need to be regenerated running a tool that looks like this:

bazel run //<project>:write_<library_name>-update-yaml

Please see this readme for more information:
{README_LINK}#2-update-yaml
#########################################################################################
"""
    elif tool.startswith("semiwrap.cmd.dat"):
        extra_error_message = f"""
#########################################################################################
An auto generation error has occurred. The most probable cause for this that the bazel
build scripts need to updated, running a tool that looks like this:

bazel run //<project>:<library_name>-generator.generate_build_info

Please see this readme for more information:
{README_LINK}#3-generate-build-info
#########################################################################################
"""

    print("-------------------------------------")
    print("Failed to run wrapped tool.")
    print(f"CWD: {os.getcwd()}")
    print(f"Tool: {tool}, Args:")
    for a in args:
        print("  ", a)
    print("-------------------------------------")

    if extra_error_message:
        print(extra_error_message)
    else:
        print(f"Please see the robotpy readme: {README_LINK}")


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
            print_failure_message(tool, args)
            print(f"sys.exit() explicitly called with a non-zero error code: {e.code}")
            sys.exit(e.code)
    except:
        print_failure_message(tool, args)
        raise


if __name__ == "__main__":
    main()
