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


def generate_hids(output_directory: Path, template_directory: Path, schema_file: Path):
    with schema_file.open(encoding="utf-8") as f:
        controllers = json.load(f)

    # C++ headers
    hdr_subdirectory = "main/native/include/wpi/driverstation"
    env = Environment(
        loader=FileSystemLoader(template_directory / hdr_subdirectory),
        autoescape=False,
        keep_trailing_newline=True,
    )
    root_path = output_directory / hdr_subdirectory
    template = env.get_template("hid.hpp.jinja")
    for controller in controllers:
        controllerName = f"{controller['ConsoleName']}Controller.hpp"
        write_file(root_path, controllerName, template.render(controller))

    # C++ files
    cpp_subdirectory = "main/native/cpp/driverstation"
    env = Environment(
        loader=FileSystemLoader(template_directory / cpp_subdirectory),
        autoescape=False,
    )
    root_path = output_directory / cpp_subdirectory
    template = env.get_template("hid.cpp.jinja")
    for controller in controllers:
        controllerName = f"{controller['ConsoleName']}Controller.cpp"
        write_file(root_path, controllerName, template.render(controller))

    # C++ simulation headers
    sim_hdr_subdirectory = "main/native/include/wpi/simulation"
    env = Environment(
        loader=FileSystemLoader(template_directory / sim_hdr_subdirectory),
        autoescape=False,
        keep_trailing_newline=True,
    )
    root_path = output_directory / sim_hdr_subdirectory
    template = env.get_template("hidsim.hpp.jinja")
    for controller in controllers:
        controllerName = f"{controller['ConsoleName']}ControllerSim.hpp"
        write_file(root_path, controllerName, template.render(controller))

    # C++ simulation files
    sim_cpp_subdirectory = "main/native/cpp/simulation"
    env = Environment(
        loader=FileSystemLoader(template_directory / sim_cpp_subdirectory),
        autoescape=False,
    )
    root_path = output_directory / sim_cpp_subdirectory
    template = env.get_template("hidsim.cpp.jinja")
    for controller in controllers:
        controllerName = f"{controller['ConsoleName']}ControllerSim.cpp"
        write_file(root_path, controllerName, template.render(controller))


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = make_arg_parser(dirname, dirname.parent)
    add_jinja_args(parser, dirname, "wpilibj/src/generate/hids.json")
    args = parser.parse_args()

    generate_hids(args.output_directory, args.template_root, args.schema_file)


if __name__ == "__main__":
    main()
