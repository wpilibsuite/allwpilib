#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import json
import re
from pathlib import Path

from jinja2 import Environment, FileSystemLoader


def write_controller_file(output_dir: Path, controller_name: str, contents: str):
    output_dir.mkdir(parents=True, exist_ok=True)
    output_file = output_dir / controller_name
    output_file.write_text(contents, encoding="utf-8", newline="\n")


def generate_hids(output_directory: Path, template_directory: Path, schema_file: Path):
    with schema_file.open(encoding="utf-8") as f:
        controllers = json.load(f)

    # Java files
    java_subdirectory = "main/java/org/wpilib/command2/button"
    env = Environment(
        loader=FileSystemLoader(template_directory / "main/java"),
        autoescape=False,
        keep_trailing_newline=True,
    )
    root_path = output_directory / java_subdirectory
    template = env.get_template("commandhid.java.jinja")
    for controller in controllers:
        controllerName = f"Command{controller['ConsoleName']}Controller.java"
        output = template.render(controller)
        write_controller_file(root_path, controllerName, output)

    # C++ headers
    hdr_subdirectory = "main/native/include/wpi/commands2/button"
    env = Environment(
        loader=FileSystemLoader(template_directory / hdr_subdirectory),
        autoescape=False,
        keep_trailing_newline=True,
    )
    root_path = output_directory / hdr_subdirectory
    template = env.get_template("commandhid.hpp.jinja")
    for controller in controllers:
        controllerName = f"Command{controller['ConsoleName']}Controller.hpp"
        output = template.render(controller)
        write_controller_file(root_path, controllerName, output)

    # C++ files
    cpp_subdirectory = "main/native/cpp/wpi/commands2/button"
    env = Environment(
        loader=FileSystemLoader(template_directory / cpp_subdirectory),
        autoescape=False,
    )
    root_path = output_directory / cpp_subdirectory
    template = env.get_template("commandhid.cpp.jinja")
    for controller in controllers:
        controllerName = f"Command{controller['ConsoleName']}Controller.cpp"
        output = template.render(controller)
        write_controller_file(root_path, controllerName, output)


def _capitalize_first(name: str) -> str:
    return name[0].upper() + name[1:]


def _display_name(name: str) -> str:
    name = re.sub(r"([a-z0-9])([A-Z])", r"\1 \2", name)
    name = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1 \2", name)
    name = re.sub(r"([A-Za-z])([0-9])", r"\1 \2", name)
    return name[0].upper() + name[1:]


def _is_trigger_axis(name: str) -> bool:
    return name.endswith("TriggerAxis") or name in {
        "L2Axis",
        "R2Axis",
        "ZLAxis",
        "ZRAxis",
        "LTriggerAxis",
        "RTriggerAxis",
    }


def _normalize_command_mapping(mapping: dict[str, int]):
    return [
        {
            "Name": name,
            "MethodName": _capitalize_first(name),
            "DocName": _display_name(name),
            "value": value,
        }
        for name, value in mapping.items()
    ]


def _normalize_new_ds_command_controller(controller: dict):
    buttons = _normalize_command_mapping(controller["buttons"])
    axes = _normalize_command_mapping(controller["axes"])
    button_names = {button["Name"] for button in buttons}

    trigger_axes = []
    for axis in axes:
        if not _is_trigger_axis(axis["Name"]):
            continue

        name = axis["Name"]
        trigger_name = name[:-4] if name.endswith("Axis") else name
        trigger_axes.append(
            {
                "Name": trigger_name,
                "MethodName": _capitalize_first(trigger_name),
                "DocName": _display_name(trigger_name),
                "AxisName": axis["Name"],
                "AxisMethodName": axis["MethodName"],
                "AxisDocName": axis["DocName"],
                "HasDefaultThresholdMethod": trigger_name not in button_names,
            }
        )

    normalized = dict(controller)
    normalized["buttons"] = buttons
    normalized["axes"] = axes
    normalized["triggerAxes"] = trigger_axes
    return normalized


def generate_new_ds_hids(
    output_directory: Path, template_directory: Path, schema_file: Path
):
    with schema_file.open(encoding="utf-8") as f:
        controllers = [
            _normalize_new_ds_command_controller(controller)
            for controller in json.load(f)
        ]

    # Java files
    java_subdirectory = "main/java/org/wpilib/command2/button"
    env = Environment(
        loader=FileSystemLoader(template_directory / "main/java"),
        autoescape=False,
        keep_trailing_newline=True,
    )
    root_path = output_directory / java_subdirectory
    template = env.get_template("new_ds_commandhid.java.jinja")
    for controller in controllers:
        controller_name = f"Command{controller['ClassName']}Controller.java"
        output = template.render(controller)
        write_controller_file(root_path, controller_name, output)

    # C++ headers
    hdr_subdirectory = "main/native/include/wpi/commands2/button"
    env = Environment(
        loader=FileSystemLoader(template_directory / hdr_subdirectory),
        autoescape=False,
        keep_trailing_newline=True,
    )
    root_path = output_directory / hdr_subdirectory
    template = env.get_template("new_ds_commandhid.hpp.jinja")
    for controller in controllers:
        controller_name = f"Command{controller['ClassName']}Controller.hpp"
        output = template.render(controller)
        write_controller_file(root_path, controller_name, output)

    # C++ files
    cpp_subdirectory = "main/native/cpp/wpi/commands2/button"
    env = Environment(
        loader=FileSystemLoader(template_directory / cpp_subdirectory),
        autoescape=False,
        keep_trailing_newline=True,
    )
    root_path = output_directory / cpp_subdirectory
    template = env.get_template("new_ds_commandhid.cpp.jinja")
    for controller in controllers:
        controller_name = f"Command{controller['ClassName']}Controller.cpp"
        output = template.render(controller)
        write_controller_file(root_path, controller_name, output)


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
    parser.add_argument(
        "--schema_file",
        help="Optional. If set, will use this file for the joystick schema",
        default="wpilibj/src/generate/hids.json",
        type=Path,
    )
    parser.add_argument(
        "--new_ds_schema_file",
        help="Optional. If set, will use this file for the new Driver Station HID schema",
        default="wpilibj/src/generate/new_ds_hids.json",
        type=Path,
    )
    args = parser.parse_args()

    generate_hids(args.output_directory, args.template_root, args.schema_file)
    generate_new_ds_hids(
        args.output_directory, args.template_root, args.new_ds_schema_file
    )


if __name__ == "__main__":
    main()
