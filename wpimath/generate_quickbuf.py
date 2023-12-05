#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
import os.path
import subprocess
import sys
from glob import glob

if __name__ == "__main__":
    proto_files = glob("wpimath/src/main/proto/*.proto")
    for path in proto_files:
        absolute_filename = os.path.abspath(path)
        absolute_dir, filename = os.path.split(absolute_filename)
        subprocess.run(
            [
                sys.argv[1],
                f"--plugin=protoc-gen-quickbuf={sys.argv[2]}",
                f"--quickbuf_out=gen_descriptors=true:{os.path.abspath('./wpimath/src/generated/main/java')}",
                f"-I{absolute_dir}",
                absolute_filename,
            ]
        )
    java_files = glob("wpimath/src/generated/main/java/edu/wpi/first/math/proto/*.java")
    for java_file in java_files:
        with open(java_file) as file:
            content = file.read()
        with open(java_file, "tw") as file:
            file.write(
                "// Copyright (c) FIRST and other WPILib contributors.\n// Open Source Software; you can modify and/or share it under the terms of\n// the WPILib BSD license file in the root directory of this project.\n"
                + content
            )
