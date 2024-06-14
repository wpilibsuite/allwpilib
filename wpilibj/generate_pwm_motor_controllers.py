#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
import argparse
import json
import sys
import os

from jinja2 import Environment, FileSystemLoader


def render_template(template, output_dir, filename, controller):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    with open(os.path.join(output_dir, filename), "w", newline="\n") as f:
        f.write(template.render(controller))


def generate_pwm_motor_controllers(output_root, template_root):
    with open(os.path.join(template_root, "pwm_motor_controllers.json")) as f:
        controllers = json.load(f)

    env = Environment(
        loader=FileSystemLoader(f"{template_root}"),
        autoescape=False,
        keep_trailing_newline=True,
    )

    rootPath = f"{output_root}/main/java/edu/wpi/first/wpilibj/motorcontrol"
    template = env.get_template("pwm_motor_controller.java.jinja")

    for controller in controllers:
        controller_name = os.path.basename(f"{controller['name']}.java")
        render_template(template, rootPath, controller_name, controller)


def main(argv):
    dirname, _ = os.path.split(os.path.abspath(__file__))

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        help="Optional. If set, will output the generated files to this directory, otherwise it will use a path relative to the script",
        default=os.path.join(dirname, "src/generated"),
    )
    parser.add_argument(
        "--template_root",
        help="Optional. If set, will use this directory as the root for the jinja templates",
        default=os.path.join(dirname, "src/generate"),
    )
    args = parser.parse_args(argv)

    generate_pwm_motor_controllers(args.output_directory, args.template_root)


if __name__ == "__main__":
    main(sys.argv[1:])
