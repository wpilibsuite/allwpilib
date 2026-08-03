#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import json
import os
import sys
from pathlib import Path
from typing import Any, Dict

# When invoked directly, Python puts the script directory on sys.path.
# Add the repo root so absolute package imports still work.
sys.path.insert(0, str(Path(__file__).absolute().parent.parent))

from jinja2 import Environment, FileSystemLoader
from shared.generation import write_file, add_jinja_args, make_arg_parser


def generate_cpp_headers(
    output_root: Path, template_root: Path, pwm_motor_controllers: Dict[str, Any]
):
    header_template_root = template_root / "main/native/include/wpi/hardware/motor"
    env = Environment(
        loader=FileSystemLoader(header_template_root),
        autoescape=False,
        keep_trailing_newline=True,
    )

    root_path = output_root / "main/native/include/wpi/hardware/motor"
    template = env.get_template("pwm_motor_controller.hpp.jinja")

    for controller in pwm_motor_controllers:
        controller_name = os.path.basename(f"{controller['name']}.hpp")
        write_file(root_path, controller_name, template.render(controller))


def generate_cpp_sources(output_root, template_root, pwm_motor_controllers):
    cpp_template_root = str(template_root / "main/native/cpp/hardware/motor")
    env = Environment(
        loader=FileSystemLoader(cpp_template_root),
        autoescape=False,
        keep_trailing_newline=True,
    )

    root_path = output_root / "main/native/cpp/hardware/motor"
    template = env.get_template("pwm_motor_controller.cpp.jinja")

    for controller in pwm_motor_controllers:
        controller_name = os.path.basename(f"{controller['name']}.cpp")
        write_file(root_path, controller_name, template.render(controller))


def generate_pwm_motor_controllers(
    output_root: Path, template_root: Path, schema_file: Path
):
    with schema_file.open(encoding="utf-8") as f:
        controllers = json.load(f)

    generate_cpp_headers(output_root, template_root, controllers)
    generate_cpp_sources(output_root, template_root, controllers)


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = make_arg_parser(dirname, dirname.parent)
    add_jinja_args(parser, dirname, "wpilibj/src/generate/pwm_motor_controllers.json")
    args = parser.parse_args()

    generate_pwm_motor_controllers(
        args.output_directory, args.template_root, args.schema_file
    )


if __name__ == "__main__":
    main()
