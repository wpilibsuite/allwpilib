#!/usr/bin/env python3

import pathlib
import sys
import tomllib
from collections.abc import Sequence


def _validate_example_list(root: pathlib.Path, expected_dirs: Sequence[str]) -> None:
    expected = {f"{name}/robot.py" for name in expected_dirs}
    actual = {p.relative_to(root).as_posix() for p in root.rglob("robot.py")}

    if expected == actual:
        return

    missing = sorted(expected - actual)
    extra = sorted(actual - expected)
    for path in missing:
        print(f"Missing: {path}")
    for path in extra:
        print(f"Extra: {path}")
    sys.exit("ERROR: Not every robot.py file is in the list of tests!")


def main(config_filename: str) -> None:
    config_path = pathlib.Path(config_filename)
    root = config_path.parent

    with config_path.open("rb") as f:
        data = tomllib.load(f)

    tests: dict[str, list[str]] = data["tests"]
    base_tests: list[str] = tests["base"]
    ignored_tests: list[str] = tests["ignored"]
    _validate_example_list(root, [*base_tests, *ignored_tests])


if __name__ == "__main__":
    main(config_filename=sys.argv[1])
