#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import json
from pathlib import Path

from jinja2 import Environment, FileSystemLoader


def write_controller_file(output_dir: Path, controller_name: str, contents: str):
    output_dir.mkdir(parents=True, exist_ok=True)
    output_file = output_dir / controller_name
    output_file.write_text(contents, encoding="utf-8", newline="\n")


def generate_hids(output_directory: Path, template_directory: Path):
    with (template_directory / "hids.json").open(encoding="utf-8") as f:
        controllers = json.load(f)

    # Java files
    env = Environment(
        loader=FileSystemLoader(template_directory / "main/java"),
        autoescape=False,
        keep_trailing_newline=True,
    )
    rootPath = output_directory / "main/java/edu/wpi/first/wpilibj"
    template = env.get_template("hid.java.jinja")
    for controller in controllers:
        controllerName = f"{controller['ConsoleName']}Controller.java"
        output = template.render(controller)
        write_controller_file(rootPath, controllerName, output)

    # Java simulation files
    rootPath = output_directory / "main/java/edu/wpi/first/wpilibj/simulation"
    template = env.get_template("hidsim.java.jinja")
    for controller in controllers:
        controllerName = f"{controller['ConsoleName']}ControllerSim.java"
        output = template.render(controller)
        write_controller_file(rootPath, controllerName, output)


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

    generate_hids(args.output_directory, args.template_root)


if __name__ == "__main__":
    main()
