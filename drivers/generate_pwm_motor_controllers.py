#!/usr/bin/env python

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import json
from pathlib import Path
from typing import Any

from jinja2 import Environment, FileSystemLoader
from jinja2.environment import Template


def render_template(
    template: Template, output_dir: Path, filename: str, controller: dict[str, Any]
):
    output_dir.mkdir(parents=True, exist_ok=True)
    (output_dir / filename).write_text(
        template.render(controller), encoding="utf-8", newline="\n"
    )


def generate_pwm_motor_controllers(output_root: Path, template_root: Path):
    with (template_root / "pwm_motor_controllers.json").open(encoding="utf-8") as f:
        controllers = json.load(f)

    template_paths = (
        (
            "main/java",
            "pwm_motor_controller.java.jinja",
            output_root / "main/java/org/wpilib/drivers/motor",
            ".java",
        ),
        (
            "main/native/include",
            "pwm_motor_controller.hpp.jinja",
            output_root / "main/native/include/wpi/drivers/motor",
            ".hpp",
        ),
        (
            "main/native/cpp",
            "pwm_motor_controller.cpp.jinja",
            output_root / "main/native/cpp/motor",
            ".cpp",
        ),
    )

    for template_dir, template_name, output_dir, suffix in template_paths:
        env = Environment(
            loader=FileSystemLoader(str(template_root / template_dir)),
            autoescape=False,
            keep_trailing_newline=True,
        )
        template = env.get_template(template_name)
        for controller in controllers:
            render_template(
                template, output_dir, f"{controller['name']}{suffix}", controller
            )


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        help="Optional output directory for generated files",
        default=dirname / "src/generated",
        type=Path,
    )
    parser.add_argument(
        "--template_root",
        help="Optional root directory for the schema and Jinja templates",
        default=dirname / "src/generate",
        type=Path,
    )
    args = parser.parse_args()

    generate_pwm_motor_controllers(args.output_directory, args.template_root)


if __name__ == "__main__":
    main()
