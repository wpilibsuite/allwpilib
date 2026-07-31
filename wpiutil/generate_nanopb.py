#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import sys
from pathlib import Path

# When invoked directly, Python puts the script directory on sys.path.
# Add the repo root so absolute package imports still work.
sys.path.insert(0, str(Path(__file__).absolute().parent.parent))

from shared.generation import make_arg_parser, generate_nanopb, GeneratorTypes


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = make_arg_parser(dirname, dirname.parent, GeneratorTypes.NANOPB)
    parser.set_defaults(
        proto_directory=dirname / "src/test/proto",
        output_directory=dirname / "src/generated/test/native/cpp",
    )
    args = parser.parse_args()

    generate_nanopb(args.nanopb, args.output_directory, args.proto_directory)


if __name__ == "__main__":
    main()
