import logging
import os
from os.path import abspath
import inspect
import pathlib
import sys
import tomllib
import typing

import wpilib

import pytest

from ..testing import pytest_plugin

# TODO: setting the plugins so that the end user can invoke pytest directly
# could be a useful thing. Will have to consider that later.

logger = logging.getLogger("test")


class _TryAgain(Exception):
    pass


#
# main test class
#


class RobotTest:
    """
    Executes unit tests on the robot code using a special pytest plugin
    """

    def __init__(self, parser=None):
        if parser:
            parser.add_argument(
                "--builtin",
                default=False,
                action="store_true",
                help="Use builtin tests if no tests are specified",
            )
            isolation_group = parser.add_mutually_exclusive_group()
            isolation_group.add_argument(
                "--isolated",
                default=None,
                dest="isolated",
                action="store_true",
                help="Run each test in a separate robot process (default). Set `tool.robotpy.testing.isolated` in your pyproject.toml to control the default",
            )
            isolation_group.add_argument(
                "--no-isolation",
                dest="isolated",
                action="store_false",
                help="Disable isolated test mode and run tests in-process",
            )
            parser.add_argument(
                "--coverage-mode",
                default=False,
                action="store_true",
                help="This flag is passed when trying to determine coverage",
            )
            parser.add_argument(
                "pytest_args",
                nargs="*",
                help="To pass args to pytest, specify --<space>, then the args",
            )
            parser.add_argument(
                "-j",
                "--jobs",
                type=int,
                default=-1,
                help="Maximum isolated robot processes (default: max CPUs - 1)",
            )

    def run(
        self,
        main_file: pathlib.Path,
        project_path: pathlib.Path,
        robot_class: typing.Type[wpilib.RobotBase],
        builtin: bool,
        isolated: typing.Optional[bool],
        coverage_mode: bool,
        verbose: bool,
        pytest_args: typing.List[str],
        jobs: int,
    ):
        if isolated is None:
            pyproject_path = project_path / "pyproject.toml"
            if pyproject_path.exists():
                with open(pyproject_path, "rb") as fp:
                    d = tomllib.load(fp)

                try:
                    v = d["tool"]["robotpy"]["testing"]["isolated"]
                except KeyError:
                    pass
                else:
                    if not isinstance(v, bool):
                        raise ValueError(
                            f"tool.robotpy.testing.isolated must be a boolean value (got {v})"
                        )

                    isolated = v

        if isolated is None:
            isolated = True

        try:
            return self._run_test(
                main_file,
                project_path,
                robot_class,
                builtin,
                isolated,
                coverage_mode,
                verbose,
                pytest_args,
                jobs,
            )
        except _TryAgain:
            return self._run_test(
                main_file,
                project_path,
                robot_class,
                builtin,
                isolated,
                coverage_mode,
                verbose,
                pytest_args,
                jobs,
            )

    def _run_test(
        self,
        main_file: pathlib.Path,
        project_path: pathlib.Path,
        robot_class: typing.Type[wpilib.RobotBase],
        builtin: bool,
        isolated: bool,
        coverage_mode: bool,
        verbose: bool,
        pytest_args: typing.List[str],
        jobs: int,
    ):
        # find test directory, change current directory so pytest can find the tests
        # -> assume that tests reside in tests or ../tests

        curdir = pathlib.Path.cwd().absolute()

        self.try_dirs = [
            ((project_path / "tests").absolute(), False),
            ((project_path / ".." / "tests").absolute(), True),
        ]

        for d, chdir in self.try_dirs:
            if d.exists():
                builtin = False
                if chdir:
                    os.chdir(d)
                break
        else:
            if not builtin:
                print("ERROR: Cannot run robot tests, as test directory was not found!")
                retv = self._no_tests(main_file, project_path)
                return 1

            from ..testing import robot_tests

            pytest_args.insert(0, abspath(inspect.getfile(robot_tests)))

        try:
            if isolated:
                from ..testing import pytest_isolated_tests_plugin

                retv = pytest.main(
                    pytest_args,
                    plugins=[
                        pytest_isolated_tests_plugin.IsolatedTestsPlugin(
                            robot_class, main_file, builtin, verbose, jobs
                        )
                    ],
                )
            else:
                retv = pytest.main(
                    pytest_args,
                    plugins=[
                        pytest_plugin.RobotTestingPlugin(robot_class, main_file, False)
                    ],
                )
        finally:
            os.chdir(curdir)

        # requires pytest 2.8.x
        if retv == 5:
            print()
            print("ERROR: a tests directory was found, but no tests were defined")
            retv = self._no_tests(main_file, project_path, retv)

        return retv

    def _no_tests(
        self, main_file: pathlib.Path, project_path: pathlib.Path, r: int = 1
    ):
        print()
        print("Looked for tests at:")
        for d, _ in self.try_dirs:
            print("-", d)
        print()
        print(
            "If you don't want to write your own tests, wpilib comes with generic tests"
        )
        print("that can test basic functionality of most robots. You can run them by")
        print("specifying the --builtin option.")
        print()

        if not sys.stdin.isatty():
            print(
                "Alternatively, to create a tests directory with the builtin tests, you can run:"
            )
            print()
            print("    python3 -m robotpy add-tests")
            print()
        else:
            if yesno("Create a tests directory with builtin tests now?"):
                from .cli_add_tests import AddTests

                add_tests = AddTests()
                add_tests.run(main_file, project_path)

                raise _TryAgain()

        return r


def yesno(prompt):
    """Returns True if user answers 'y'"""
    prompt += " [y/n]"
    a = ""
    while a not in ["y", "n"]:
        a = input(prompt).lower()

    return a == "y"
