#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import json
import os
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


def generate_cpp_headers(
    output_root: Path, template_root: Path, pwm_motor_controllers: Dict[str, Any]
):
    header_template_root = template_root / "main/native/include/frc/motorcontroller"
    env = Environment(
        loader=FileSystemLoader(header_template_root),
        autoescape=False,
        keep_trailing_newline=True,
    )

    root_path = output_root / "main/native/include/frc/motorcontrol"
    template = env.get_template("pwm_motor_controller.h.jinja")

    for controller in pwm_motor_controllers:
        controller_name = os.path.basename(f"{controller['name']}.h")
        render_template(template, root_path, controller_name, controller)


def generate_cpp_sources(output_root, template_root, pwm_motor_controllers):
    cpp_template_root = str(template_root / "main/native/cpp/motorcontroller")
    env = Environment(
        loader=FileSystemLoader(cpp_template_root),
        autoescape=False,
        keep_trailing_newline=True,
    )

    root_path = output_root / "main/native/cpp/motorcontrol"
    template = env.get_template("pwm_motor_controller.cpp.jinja")

    for controller in pwm_motor_controllers:
        controller_name = os.path.basename(f"{controller['name']}.cpp")
        render_template(template, root_path, controller_name, controller)


def generate_pwm_motor_controllers(
    output_root: Path, template_root: Path, schema_root: Path
):
    with (schema_root / "pwm_motor_controllers.json").open(encoding="utf-8") as f:
        controllers = json.load(f)

    generate_cpp_headers(output_root, template_root, controllers)
    generate_cpp_sources(output_root, template_root, controllers)


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
        "--schema_root",
        help="Optional. If set, will use this directory as the root for discovering the pwm controller schema",
        default=dirname / "../wpilibj/src/generate",
        type=Path,
    )
    parser.add_argument(
        "--template_root",
        help="Optional. If set, will use this directory as the root for the jinja templates",
        default=dirname / "src/generate",
        type=Path,
    )
    args = parser.parse_args()

    generate_pwm_motor_controllers(
        args.output_directory, args.template_root, args.schema_root
    )


if __name__ == "__main__":
    main()
