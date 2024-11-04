#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import json
from pathlib import Path
from typing import Any, Dict

from jinja2 import Environment, FileSystemLoader
from jinja2.environment import Template


def render_template(
    template: Template, output_dir: Path, filename: str, controller: Dict[str, Any]
):
    output_dir.mkdir(parents=True, exist_ok=True)

    output_file = output_dir / filename
    output_file.write_text(template.render(controller), encoding="utf-8", newline="\n")


def generate_pwm_motor_controllers(output_root, template_root):
    with (template_root / "pwm_motor_controllers.json").open(encoding="utf-8") as f:
        controllers = json.load(f)

    env = Environment(
        loader=FileSystemLoader(str(template_root / "main/java")),
        autoescape=False,
        keep_trailing_newline=True,
    )

    root_path = Path(output_root) / "main/java/edu/wpi/first/wpilibj/motorcontrol"
    template = env.get_template("pwm_motor_controller.java.jinja")

    for controller in controllers:
        controller_name = f"{controller['name']}.java"
        render_template(template, root_path, controller_name, controller)


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

    generate_pwm_motor_controllers(args.output_directory, args.template_root)


if __name__ == "__main__":
    main()
