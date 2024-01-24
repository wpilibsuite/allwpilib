#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import os
import sys
import argparse
from jinja2 import Environment, FileSystemLoader


def output(outPath, outfn, contents):
    if not os.path.exists(outPath):
        os.makedirs(outPath)

    outpathname = f"{outPath}/{outfn}"

    # File either doesn't exist or has different contents
    with open(outpathname, "w", newline="\n") as f:
        f.write(contents)


def main():
    MAX_NUM = 20

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        help="Optional. If set, will output the generated files to this directory, otherwise it will use a path relative to the script",
    )
    args = parser.parse_args()

    if args.output_directory:
        dirname = args.output_directory
        template_root = "wpimath"
    else:
        script_dir, _ = os.path.split(os.path.abspath(__file__))
        dirname = os.path.join(script_dir, "src", "generated")
        template_root = script_dir

    env = Environment(
        loader=FileSystemLoader(f"{template_root}/src/generate/main/java"),
        autoescape=False,
        keep_trailing_newline=True,
    )

    template = env.get_template("GenericNumber.java.jinja")
    rootPath = f"{dirname}/main/java/edu/wpi/first/math/numbers"

    for i in range(MAX_NUM + 1):
        contents = template.render(num=i)
        output(rootPath, f"N{i}.java", contents)

    template = env.get_template("Nat.java.jinja")
    rootPath = f"{dirname}/main/java/edu/wpi/first/math"
    contents = template.render(nums=range(MAX_NUM + 1))
    output(rootPath, "Nat.java", contents)


if __name__ == "__main__":
    main()
