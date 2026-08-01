#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import json
import sys
from pathlib import Path


# When invoked directly, Python puts the script directory on sys.path.
# Add the repo root so absolute package imports still work.
sys.path.insert(0, str(Path(__file__).absolute().parent.parent))

from jinja2 import Environment, FileSystemLoader
from shared.generation import write_file, add_jinja_args, make_arg_parser


def generate_hids(output_directory: Path, template_directory: Path):
    with (template_directory / "hids.json").open(encoding="utf-8") as f:
        controllers = json.load(f)

    # Java files
    env = Environment(
        loader=FileSystemLoader(template_directory / "main/java"),
        autoescape=False,
        keep_trailing_newline=True,
    )
    rootPath = output_directory / "main/java/org/wpilib/driverstation"
    template = env.get_template("hid.java.jinja")
    for controller in controllers:
        controllerName = f"{controller['ConsoleName']}Controller.java"
        write_file(rootPath, controllerName, template.render(controller))

    # Java simulation files
    rootPath = output_directory / "main/java/org/wpilib/simulation"
    template = env.get_template("hidsim.java.jinja")
    for controller in controllers:
        controllerName = f"{controller['ConsoleName']}ControllerSim.java"
        write_file(rootPath, controllerName, template.render(controller))


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = make_arg_parser(dirname, dirname.parent)
    add_jinja_args(parser, dirname, None)
    args = parser.parse_args()

    generate_hids(args.output_directory, args.template_root)


if __name__ == "__main__":
    main()
