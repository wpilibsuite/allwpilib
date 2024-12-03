#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
from pathlib import Path

from jinja2 import Environment, FileSystemLoader


def output(output_dir: Path, outfn: str, contents: str):
    output_dir.mkdir(parents=True, exist_ok=True)
    output_file = output_dir / outfn
    output_file.write_text(contents, encoding="utf-8", newline="\n")


def generate_numbers(output_directory: Path, template_root: Path):
    MAX_NUM = 20

    env = Environment(
        loader=FileSystemLoader(template_root / "main/java"),
        autoescape=False,
        keep_trailing_newline=True,
    )

    template = env.get_template("GenericNumber.java.jinja")
    rootPath = output_directory / "main/java/edu/wpi/first/math/numbers"

    for i in range(MAX_NUM + 1):
        contents = template.render(num=i)
        output(rootPath, f"N{i}.java", contents)

    template = env.get_template("Nat.java.jinja")
    rootPath = output_directory / "main/java/edu/wpi/first/math"
    contents = template.render(nums=range(MAX_NUM + 1))
    output(rootPath, "Nat.java", contents)


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        help="Optional. If set, will output the generated files to this directory, otherwise it will use a path relative to the script",
        default=dirname / "src/generated",
        type=Path,
    )
    parser.add_argument(
        "--template_root",
        help="Optional. If set, will use this directory as the root for the jinja templates",
        default=dirname / "src/generate",
        type=Path,
    )
    args = parser.parse_args()

    generate_numbers(args.output_directory, args.template_root)


if __name__ == "__main__":
    main()
