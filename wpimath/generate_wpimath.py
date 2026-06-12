# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import os
import sys
from pathlib import Path

from wpimath.generate_nanopb import generate_nanopb
from wpimath.generate_numbers import generate_numbers
from wpimath.generate_quickbuf import generate_quickbuf


def main(argv):
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    root_path = dirname.parent
    nanopb_path = os.path.join(
        root_path,
        "wpiutil",
        "src",
        "main",
        "native",
        "thirdparty",
        "nanopb",
        "generator",
        "nanopb_generator.py",
    )

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
        "--proto_directory",
        help="Optional. If set, will use this directory to glob for protobuf files",
        default=dirname / "src/main/proto",
        type=Path,
    )
    parser.add_argument(
        "--protoc",
        help="Protoc executable command",
        default="protoc",
    )
    parser.add_argument(
        "--nanopb_generator", help="Nanopb generator command", default=nanopb_path
    )
    parser.add_argument(
        "--quickbuf_plugin",
        help="Path to the quickbuf protoc plugin",
        required=True,
    )
    args = parser.parse_args(argv)

    generate_numbers(args.output_directory, args.template_root)
    generate_nanopb(
        args.nanopb_generator,
        args.output_directory / "main/native/cpp/wpimath/protobuf",
        args.proto_directory,
    )
    generate_quickbuf(
        args.protoc,
        args.quickbuf_plugin,
        args.output_directory / "main/java",
        args.proto_directory,
    )


if __name__ == "__main__":
    main(sys.argv[1:])
