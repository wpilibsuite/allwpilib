#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import sys
from pathlib import Path

# When invoked directly, Python puts the script directory on sys.path.
# Add the repo root so absolute package imports still work.
sys.path.insert(0, str(Path(__file__).absolute().parent.parent))

from fields.generate_enums import generate_enums
from fields.generate_nanopb import generate_nanopb
from fields.generate_quickbuf import generate_quickbuf
from shared.generation import GeneratorTypes, add_jinja_args, make_arg_parser


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = make_arg_parser(
        dirname, dirname.parent, GeneratorTypes.NANOPB | GeneratorTypes.QUICKBUF
    )
    add_jinja_args(parser, dirname, None)
    parser.add_argument(
        "--resources_root",
        default=dirname / "src/main/native/resources",
        type=Path,
        help="Root directory containing org/wpilib/fields JSON resources",
    )
    parser.set_defaults(proto_path=[dirname.parent / "wpimath/src/main/proto"])
    args = parser.parse_args()

    generate_enums(args.output_directory, args.resources_root, args.template_root)
    generate_nanopb(
        args.nanopb,
        args.output_directory / "main/native/cpp",
        args.output_directory / "main/native/include",
        args.proto_directory,
        args.proto_path,
    )
    generate_quickbuf(
        args.protoc,
        args.quickbuf_plugin,
        args.output_directory / "main/java",
        args.proto_directory,
        args.proto_path,
    )


if __name__ == "__main__":
    main()
