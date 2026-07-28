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


def _normalize_mapping(mapping: dict[str, int]):
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


def _supported_outputs(controller: dict):
    supports = controller["supports"]
    outputs = []
    if supports["monoLed"]:
        outputs.append("MONO_LED")
    if supports["rgbLed"]:
        outputs.append("RGB_LED")
    if supports["rumble"]:
        outputs.append("RUMBLE")
    if supports["triggerRumble"]:
        outputs.append("TRIGGER_RUMBLE")
    return outputs


def _supported_outputs_value(outputs: list[str]):
    values = {
        "MONO_LED": 0x1,
        "RGB_LED": 0x2,
        "RUMBLE": 0x8,
        "TRIGGER_RUMBLE": 0x10,
    }
    result = 0
    for output in outputs:
        result |= values[output]
    return result


def _availability_mask(mapping: list[dict]):
    result = 0
    for entry in mapping:
        result |= 1 << entry["value"]
    return result


def _hex_literal(value: int):
    return f"0x{value:X}"


def _normalize_controller(controller: dict):
    normalized = dict(controller)
    normalized["axes"] = _normalize_mapping(controller["axes"])
    normalized["buttons"] = _normalize_mapping(controller["buttons"])
    normalized["supportedOutputs"] = _supported_outputs(controller)
    normalized["SupportedOutputsValue"] = _supported_outputs_value(
        normalized["supportedOutputs"]
    )
    normalized["AxesMaximumIndex"] = (
        max(axis["value"] for axis in normalized["axes"]) + 1
        if normalized["axes"]
        else 0
    )
    normalized["ButtonsMaximumIndex"] = (
        max(button["value"] for button in normalized["buttons"]) + 1
        if normalized["buttons"]
        else 0
    )
    normalized["AxesAvailableMask"] = _hex_literal(
        _availability_mask(normalized["axes"])
    )
    normalized["ButtonsAvailableMask"] = _hex_literal(
        _availability_mask(normalized["buttons"])
    )
    return normalized


def generate_first_ds_hids(
    output_directory: Path,
    template_directory: Path,
    test_output_directory: Path | None = None,
):
    with (template_directory / "first_ds_hids.json").open(encoding="utf-8") as f:
        controllers = [_normalize_controller(controller) for controller in json.load(f)]

    env = Environment(
        loader=FileSystemLoader(template_directory / "main/java"),
        autoescape=False,
        keep_trailing_newline=True,
    )

    root_path = output_directory / "main/java/org/wpilib/driverstation"
    template = env.get_template("first_ds_hid.java.jinja")
    for controller in controllers:
        controller_name = f"{controller['ClassName']}Controller.java"
        output = template.render(controller)
        write_controller_file(root_path, controller_name, output)

    root_path = output_directory / "main/java/org/wpilib/simulation"
    template = env.get_template("first_ds_hidsim.java.jinja")
    for controller in controllers:
        controller_name = f"{controller['ClassName']}ControllerSim.java"
        output = template.render(controller)
        write_controller_file(root_path, controller_name, output)

    if test_output_directory is not None:
        env = Environment(
            loader=FileSystemLoader(template_directory / "test/java"),
            autoescape=False,
            keep_trailing_newline=True,
        )

        root_path = test_output_directory / "java/org/wpilib/driverstation"
        template = env.get_template("first_ds_hid_test.java.jinja")
        for controller in controllers:
            controller_name = f"{controller['ClassName']}ControllerTest.java"
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
        "--test_output_directory",
        help="Optional. If set, will output generated tests to this directory",
        default=dirname / "src/generated/test",
        type=Path,
    )
    args = parser.parse_args()

    test_output_directory = (
        None
        if args.test_output_directory.name == "__none__"
        else args.test_output_directory
    )
    generate_first_ds_hids(
        args.output_directory, args.template_root, test_output_directory
    )


if __name__ == "__main__":
    main()
