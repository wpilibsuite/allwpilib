#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path


def generate_nanopb(nanopb: Path, output_directory: Path, proto_dir: Path):
    shutil.rmtree(output_directory.absolute(), ignore_errors=True)
    os.makedirs(output_directory.absolute())

    proto_files = proto_dir.glob("*.proto")
    for path in proto_files:
        absolute_filename = path.absolute()
        subprocess.run(
            [
                sys.executable,
                nanopb,
                f"-I{absolute_filename.parent}",
                f"-D{output_directory.absolute()}",
                "-S.cpp",
                "-e.npb",
                absolute_filename,
            ],
            check=True,
        )
    java_files = (output_directory).glob("*")
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
        "--nanopb",
        help="Nanopb generator command",
        default=nanopb_path,
    )
    parser.add_argument(
        "--output_directory",
        help="Optional. If set, will output the generated files to this directory, otherwise it will use a path relative to the script",
        default=dirname / "src/generated/main/native/cpp/wpimath/protobuf",
        type=Path,
    )
    parser.add_argument(
        "--proto_directory",
        help="Optional. If set, will use this directory to glob for protobuf files",
        default=dirname / "src/main/proto",
        type=Path,
    )
    args = parser.parse_args()

    generate_nanopb(args.nanopb, args.output_directory, args.proto_directory)


if __name__ == "__main__":
    main()
