#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import os
from pathlib import Path

from generate_first_ds_hids import generate_first_ds_hids
from generate_hids import generate_hids
from generate_pwm_motor_controllers import generate_pwm_motor_controllers


def main():
    dirname, _ = os.path.split(os.path.abspath(__file__))

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        help="Optional. If set, will output the generated files to this directory, otherwise it will use a path relative to the script",
        default=os.path.join(dirname, "src/generated"),
        type=Path,
    )
    parser.add_argument(
        "--template_root",
        help="Optional. If set, will use this directory as the root for the jinja templates",
        default=os.path.join(dirname, "src/generate"),
        type=Path,
    )
    parser.add_argument(
        "--test_output_directory",
        help="Optional. If set, will output generated tests to this directory",
        default=os.path.join(dirname, "src/generated/test"),
        type=Path,
    )
    args = parser.parse_args()

    generate_hids(args.output_directory, args.template_root)
    test_output_directory = (
        None
        if args.test_output_directory.name == "__none__"
        else args.test_output_directory
    )
    generate_first_ds_hids(
        args.output_directory, args.template_root, test_output_directory
    )
    generate_pwm_motor_controllers(args.output_directory, args.template_root)


if __name__ == "__main__":
    main()
