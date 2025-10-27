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


def generate_hids(output_directory: Path, template_directory: Path, schema_file: Path):
    with schema_file.open(encoding="utf-8") as f:
        controllers = json.load(f)

    # Java files
    java_subdirectory = "main/java/edu/wpi/first/wpilibj2/command/button"
    env = Environment(
        loader=FileSystemLoader(template_directory / "main/java"),
        autoescape=False,
        keep_trailing_newline=True,
    )
    root_path = output_directory / java_subdirectory
    template = env.get_template("commandhid.java.jinja")
    for controller in controllers:
        controllerName = f"Command{controller['ConsoleName']}Controller.java"
        output = template.render(controller)
        write_controller_file(root_path, controllerName, output)

    # C++ headers
    hdr_subdirectory = "main/native/include/frc2/command/button"
    env = Environment(
        loader=FileSystemLoader(template_directory / hdr_subdirectory),
        autoescape=False,
        keep_trailing_newline=True,
    )
    root_path = output_directory / hdr_subdirectory
    template = env.get_template("commandhid.h.jinja")
    for controller in controllers:
        controllerName = f"Command{controller['ConsoleName']}Controller.h"
        output = template.render(controller)
        write_controller_file(root_path, controllerName, output)

    # C++ files
    cpp_subdirectory = "main/native/cpp/frc2/command/button"
    env = Environment(
        loader=FileSystemLoader(template_directory / cpp_subdirectory),
        autoescape=False,
    )
    root_path = output_directory / cpp_subdirectory
    template = env.get_template("commandhid.cpp.jinja")
    for controller in controllers:
        controllerName = f"Command{controller['ConsoleName']}Controller.cpp"
        output = template.render(controller)
        write_controller_file(root_path, controllerName, output)


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
    parser.add_argument(
        "--schema_file",
        help="Optional. If set, will use this file for the joystick schema",
        default="wpilibj/src/generate/hids.json",
        type=Path,
    )
    args = parser.parse_args()

    generate_hids(args.output_directory, args.template_root, args.schema_file)


if __name__ == "__main__":
    main()
