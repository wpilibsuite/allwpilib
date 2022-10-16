# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import os
import sys
import argparse
from jinja2 import Environment, FileSystemLoader


def output(outPath, outfn, contents):
    if not os.path.exists(outPath):
        os.makedirs(outPath)

    outpathname = f"{outPath}/{outfn}"

    if os.path.exists(outpathname):
        with open(outpathname, "r") as f:
            if f.read() == contents:
                return

    # File either doesn't exist or has different contents
    with open(outpathname, "w") as f:
        f.write(contents)


def main():
    MAX_NUM = 20

    parser = argparse.ArgumentParser()

    is_bazel = "RUNFILES_MANIFEST_FILE" in os.environ or "RUNFILES_DIR" in os.environ
    if is_bazel:
        parser.add_argument("--output_files", nargs="+")
    else:
        parser.add_argument("--cmake_binary_dir")
    args = parser.parse_args()

    if is_bazel:
        dirname = "wpimath"

        files_to_generate = args.output_files
        base_name = os.path.dirname(files_to_generate[0])
        generation_root = "/".join(base_name.split("/")[0:4]) + "/generated"

        if len(files_to_generate) != (MAX_NUM + 1 + 1):
            raise Exception(
                f"Bazel is generating an unexpected number of files {len(files_to_generate)}"
            )
    else:
        dirname, _ = os.path.split(os.path.abspath(__file__))
        generation_root = f"{args.cmake_binary_dir}/generated/main"

    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate"),
        autoescape=False,
        keep_trailing_newline=True,
    )

    template = env.get_template("GenericNumber.java.jinja")
    rootPath = f"{generation_root}/java/edu/wpi/first/math/numbers"

    for i in range(MAX_NUM + 1):
        contents = template.render(num=i)
        output(rootPath, f"N{i}.java", contents)

    template = env.get_template("Nat.java.jinja")
    rootPath = f"{generation_root}/java/edu/wpi/first/math"
    contents = template.render(nums=range(MAX_NUM + 1))
    output(rootPath, "Nat.java", contents)


if __name__ == "__main__":
    main()
