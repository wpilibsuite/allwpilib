#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import sys
from pathlib import Path

# When invoked directly, Python puts the script directory on sys.path.
# Add the repo root so absolute package imports still work.
sys.path.insert(0, str(Path(__file__).absolute().parent.parent))

from shared.generation import generate_python_protobuf


def main(argv):
    dirname = Path(__file__).resolve().parent

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        help="Where to write the generated Python protobuf modules.",
        default=dirname / "src/main/python/wpimath/protobuf/_generated",
        type=Path,
    )
    parser.add_argument(
        "--proto_directory",
        help="Directory containing the .proto files.",
        default=dirname / "src/main/proto",
        type=Path,
    )
    parser.add_argument(
        "--protoc",
        help="The protoc command to run, e.g. 'python -m grpc_tools.protoc'.",
        nargs="+",
        required=True,
    )
    args = parser.parse_args(argv)

    generate_python_protobuf(args.protoc, args.output_directory, args.proto_directory)


if __name__ == "__main__":
    main(sys.argv[1:])
