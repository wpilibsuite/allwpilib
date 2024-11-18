#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import subprocess
from pathlib import Path


def generate_quickbuf(
    protoc, quickbuf_plugin: Path, output_directory: Path, proto_dir: Path
):
    proto_files = proto_dir.glob("*.proto")
    for path in proto_files:
        absolute_filename = path.absolute()
        subprocess.run(
            [
                protoc,
                f"--plugin=protoc-gen-quickbuf={quickbuf_plugin}",
                f"--quickbuf_out=gen_descriptors=true:{output_directory.absolute()}",
                f"-I{absolute_filename.parent}",
                absolute_filename,
            ]
        )
    java_files = (output_directory / "edu/wpi/first/math/proto").glob("*.java")
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
        "--protoc",
        help="Protoc executable command",
        default="protoc",
    )
    parser.add_argument(
        "--quickbuf_plugin",
        help="Path to the quickbuf protoc plugin",
        required=True,
    )
    parser.add_argument(
        "--output_directory",
        help="Optional. If set, will output the generated files to this directory, otherwise it will use a path relative to the script",
        default=dirname / "src/generated/main/java",
        type=Path,
    )
    parser.add_argument(
        "--proto_directory",
        help="Optional. If set, will use this directory to glob for protobuf files",
        default=dirname / "src/main/proto",
        type=Path,
    )
    args = parser.parse_args()

    generate_quickbuf(
        args.protoc, args.quickbuf_plugin, args.output_directory, args.proto_directory
    )


if __name__ == "__main__":
    main()
