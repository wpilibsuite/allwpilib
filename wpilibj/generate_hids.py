#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import json
import re
import sys
from pathlib import Path

# When invoked directly, Python puts the script directory on sys.path.
# Add the repo root so absolute package imports still work.
sys.path.insert(0, str(Path(__file__).absolute().parent.parent))

from jinja2 import Environment, FileSystemLoader

from shared.generation import add_jinja_args, make_arg_parser, write_file


def _capitalize_first(name: str):
    return name[0].upper() + name[1:]


def _constant_name(name: str):
    name = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1_\2", name)
    name = re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", name)
    name = re.sub(r"([a-z])([0-9])", r"\1_\2", name)
    return name.upper()


def _with_constant_name(entry: dict):
    normalized = dict(entry)
    normalized["ConstantName"] = _constant_name(entry["name"])
    normalized["MethodName"] = _capitalize_first(entry["name"])
    return normalized


def _normalize_controller(controller: dict):
    normalized = dict(controller)
    normalized["buttons"] = [
        _with_constant_name(button) for button in controller["buttons"]
    ]
    normalized["triggers"] = [
        _with_constant_name(trigger) for trigger in controller.get("triggers", [])
    ]
    for trigger in normalized["triggers"]:
        trigger["StringName"] = trigger["MethodName"]
        if trigger["name"].endswith(controller["AxisNameSuffix"]):
            trigger["StringName"] += "Axis"
    normalized["sticks"] = [
        {
            **stick,
            "ConstantName": "_".join(part.upper() for part in stick["NameParts"]),
            "MethodName": "".join(
                _capitalize_first(part) for part in stick["NameParts"]
            ),
        }
        for stick in controller["sticks"]
    ]
    return normalized


def generate_hids(output_directory: Path, template_directory: Path):
    with (template_directory / "hids.json").open(encoding="utf-8") as f:
        controllers = [_normalize_controller(controller) for controller in json.load(f)]

    # Java files
    env = Environment(
        loader=FileSystemLoader(template_directory / "main/java"),
        autoescape=False,
        keep_trailing_newline=True,
    )
    rootPath = output_directory / "main/java/org/wpilib/driverstation"
    template = env.get_template("hid.java.jinja")
    for controller in controllers:
        controllerName = f"{controller['ConsoleName']}Controller.java"
        write_file(rootPath, controllerName, template.render(controller))

    # Java simulation files
    rootPath = output_directory / "main/java/org/wpilib/simulation"
    template = env.get_template("hidsim.java.jinja")
    for controller in controllers:
        controllerName = f"{controller['ConsoleName']}ControllerSim.java"
        write_file(rootPath, controllerName, template.render(controller))


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = make_arg_parser(dirname, dirname.parent)
    add_jinja_args(parser, dirname, None)
    args = parser.parse_args()

    generate_hids(args.output_directory, args.template_root)


if __name__ == "__main__":
    main()
