from glob import glob
import os.path
import re
import subprocess
import sys

if __name__ == "__main__":
    proto_files = glob("wpimath/src/main/proto/*.proto")
    for path in proto_files:
        absolute_filename = os.path.abspath(path)
        absolute_dir, filename = os.path.split(absolute_filename)
        file, _ = os.path.splitext(filename)
        path.split()
        pattern = re.compile("[A-Za-z_]+|[0-9]+")
        filename_parts: list[str] = pattern.findall(file)
        filename_parts = [part.capitalize() for part in filename_parts]
        subprocess.run(
            f"{sys.argv[1]} --plugin=protoc-gen-quickbuf={sys.argv[2]} --quickbuf_out=gen_descriptors=true:{os.path.abspath('./wpimath/src/generated/main/java')} -I{absolute_dir} {absolute_filename}"
        )
    java_files = glob("wpimath/src/generated/main/java/edu/wpi/first/math/proto/*.java")
    for java_file in java_files:
        with open(java_file) as file:
            content = file.read()
        with open(java_file, "tw") as file:
            file.write(
                "// Copyright (c) FIRST and other WPILib contributors.\n// Open Source Software; you can modify and/or share it under the terms of\n// the WPILib BSD license file in the root directory of this project.\n\n"
                + content
            )
