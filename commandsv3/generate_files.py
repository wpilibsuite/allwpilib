#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import json
import subprocess
from pathlib import Path

from jinja2 import Environment, FileSystemLoader


def write_controller_file(output_dir: Path, controller_name: str, contents: str):
    output_dir.mkdir(parents=True, exist_ok=True)
    output_file = output_dir / controller_name
    output_file.write_text(contents, encoding="utf-8", newline="\n")
    print("Writing to ", output_file)


def generate_hids(output_directory: Path, template_directory: Path, schema_file: Path):
    with schema_file.open(encoding="utf-8") as f:
        controllers = json.load(f)

    # Java files
    java_subdirectory = "main/java/org/wpilib/commands3/button"
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


def generate_quickbuf(
    protoc, quickbuf_plugin: Path, output_directory: Path, proto_dir: Path
):
    proto_files = proto_dir.glob("*.proto")
    for path in proto_files:
        absolute_filename = path.absolute()
        args = [protoc]
        if quickbuf_plugin:
            # Optional on macOS if using protoc-quickbuf
            args += [f"--plugin=protoc-gen-quickbuf={quickbuf_plugin}"]
        args += [
            f"--quickbuf_out=gen_descriptors=true:{output_directory.absolute()}",
            f"-I{absolute_filename.parent}",
            absolute_filename,
        ]
        subprocess.check_call(args)
    java_files = (output_directory / "org/wpilib/commands3/proto").glob("*.java")
    for java_file in java_files:
        with (java_file).open(encoding="utf-8") as f:
            content = f.read()

        java_file.write_text(
            "// Copyright (c) FIRST and other WPILib contributors.\n// Open Source Software; you can modify and/or share it under the terms of\n// the WPILib BSD license file in the root directory of this project.\n"
            + content,
            encoding="utf-8",
            newline="\n",
        )


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
        "--protoc",
        help="Protoc executable command",
        default="protoc",
    )
    parser.add_argument(
        "--quickbuf_plugin",
        help="Path to the quickbuf protoc plugin",
    )
    parser.add_argument(
        "--proto_directory",
        help="Optional. If set, will use this directory to glob for protobuf files",
        default=dirname / "src/main/proto",
        type=Path,
    )
    args = parser.parse_args()

    generate_hids(args.output_directory, args.template_root, args.schema_file)
    generate_quickbuf(
        args.protoc,
        args.quickbuf_plugin,
        args.output_directory / "main/java",
        args.proto_directory,
    )


if __name__ == "__main__":
    main()
