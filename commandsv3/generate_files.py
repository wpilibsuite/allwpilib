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
from shared.generation import (
    add_jinja_args,
    make_arg_parser,
    generate_quickbuf,
    write_file,
    GeneratorTypes,
)


def generate_hids(output_directory: Path, template_directory: Path, schema_file: Path):
    with schema_file.open(encoding="utf-8") as f:
        controllers = json.load(f)

    # Java files
    java_subdirectory = "main/java/org/wpilib/command3/button"
    env = Environment(
        loader=FileSystemLoader(template_directory / "main/java"),
        autoescape=False,
        keep_trailing_newline=True,
    )
    root_path = output_directory / java_subdirectory
    template = env.get_template("commandhid.java.jinja")
    for controller in controllers:
        controllerName = f"Command{controller['ConsoleName']}Controller.java"
        write_file(root_path, controllerName, template.render(controller))


def _capitalize_first(name: str) -> str:
    return name[0].upper() + name[1:]


def _display_name(name: str) -> str:
    name = re.sub(r"([a-z0-9])([A-Z])", r"\1 \2", name)
    name = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1 \2", name)
    name = re.sub(r"([A-Za-z])([0-9])", r"\1 \2", name)
    return name[0].upper() + name[1:]


def _constant_name(name: str) -> str:
    name = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1_\2", name)
    name = re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", name)
    name = re.sub(r"([a-z])([0-9])", r"\1_\2", name)
    return name.upper()


def _is_trigger_axis(name: str) -> bool:
    return name.endswith("Trigger") or name in {
        "L2",
        "R2",
        "ZL",
        "ZR",
    }


def _normalize_command_mapping(mapping: dict[str, int]):
    return [
        {
            "Name": name,
            "MethodName": _capitalize_first(name),
            "ConstantName": _constant_name(name),
            "DocName": _display_name(name),
            "value": value,
        }
        for name, value in mapping.items()
    ]


def _normalize_first_ds_command_controller(controller: dict):
    buttons = _normalize_command_mapping(controller["buttons"])
    axes = _normalize_command_mapping(controller["axes"])
    button_names = {button["Name"] for button in buttons}

    trigger_axes = []
    for axis in axes:
        if not _is_trigger_axis(axis["Name"]):
            continue

        name = axis["Name"]
        trigger_name = name
        trigger_axes.append(
            {
                "Name": trigger_name,
                "MethodName": _capitalize_first(trigger_name),
                "DocName": _display_name(trigger_name),
                "AxisName": axis["Name"],
                "AxisMethodName": axis["MethodName"],
                "AxisConstantName": axis["ConstantName"],
                "AxisDocName": axis["DocName"],
                "HasDefaultThresholdMethod": trigger_name not in button_names,
            }
        )

    normalized = dict(controller)
    normalized["buttons"] = buttons
    normalized["axes"] = axes
    normalized["triggerAxes"] = trigger_axes
    return normalized


def generate_first_ds_hids(
    output_directory: Path, template_directory: Path, schema_file: Path
):
    with schema_file.open(encoding="utf-8") as f:
        controllers = [
            _normalize_first_ds_command_controller(controller)
            for controller in json.load(f)
        ]

    java_subdirectory = "main/java/org/wpilib/command3/button"
    env = Environment(
        loader=FileSystemLoader(template_directory / "main/java"),
        autoescape=False,
        keep_trailing_newline=True,
    )
    root_path = output_directory / java_subdirectory
    template = env.get_template("first_ds_commandhid.java.jinja")
    for controller in controllers:
        controller_name = f"Command{controller['ClassName']}Controller.java"
        print(root_path, controller_name)
        write_file(root_path, controller_name, template.render(controller))


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = make_arg_parser(dirname, dirname.parent, GeneratorTypes.QUICKBUF)
    add_jinja_args(parser, dirname, "wpilibj/src/generate/hids.json")
    parser.add_argument(
        "--first_ds_schema_file",
        help="Optional. If set, will use this file for the FIRST Driver Station HID schema",
        default="wpilibj/src/generate/first_ds_hids.json",
        type=Path,
    )
    args = parser.parse_args()
    generate_hids(args.output_directory, args.template_root, args.schema_file)
    generate_first_ds_hids(
        args.output_directory, args.template_root, args.first_ds_schema_file
    )
    generate_quickbuf(
        args.protoc,
        args.quickbuf_plugin,
        args.output_directory / "main/java",
        args.proto_directory,
    )


if __name__ == "__main__":
    main()
