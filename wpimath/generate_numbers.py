#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import sys
from pathlib import Path


# When invoked directly, Python puts the script directory on sys.path.
# Add the repo root so absolute package imports still work.
sys.path.insert(0, str(Path(__file__).absolute().parent.parent))

from jinja2 import Environment, FileSystemLoader
from shared.generation import make_arg_parser, add_jinja_args, write_file


def generate_numbers(output_directory: Path, template_root: Path):
    MAX_NUM = 20

    env = Environment(
        loader=FileSystemLoader(template_root / "main/java"),
        autoescape=False,
        keep_trailing_newline=True,
    )

    template = env.get_template("GenericNumber.java.jinja")
    rootPath = output_directory / "main/java/org/wpilib/math/numbers"

    for i in range(MAX_NUM + 1):
        contents = template.render(num=i)
        write_file(rootPath, f"N{i}.java", contents)

    template = env.get_template("Nat.java.jinja")
    rootPath = output_directory / "main/java/org/wpilib/math/util"
    contents = template.render(nums=range(MAX_NUM + 1))
    write_file(rootPath, "Nat.java", contents)


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = make_arg_parser(dirname, dirname.parent)
    add_jinja_args(parser, dirname, None)
    args = parser.parse_args()

    generate_numbers(args.output_directory, args.template_root)


if __name__ == "__main__":
    main()
