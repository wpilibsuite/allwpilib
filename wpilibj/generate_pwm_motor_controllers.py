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


def generate_pwm_motor_controllers(output_root, template_root):
    with (template_root / "pwm_motor_controllers.json").open(encoding="utf-8") as f:
        controllers = json.load(f)

    env = Environment(
        loader=FileSystemLoader(str(template_root / "main/java")),
        autoescape=False,
        keep_trailing_newline=True,
    )

    root_path = Path(output_root) / "main/java/org/wpilib/hardware/motor"
    template = env.get_template("pwm_motor_controller.java.jinja")

    for controller in controllers:
        controller_name = f"{controller['name']}.java"
        write_file(root_path, controller_name, template.render(controller))


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = make_arg_parser(dirname, dirname.parent)
    add_jinja_args(parser, dirname, None)
    args = parser.parse_args()

    generate_pwm_motor_controllers(args.output_directory, args.template_root)


if __name__ == "__main__":
    main()
