# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import os
import sys
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

    dirname, _ = os.path.split(os.path.abspath(__file__))
    cmake_binary_dir = sys.argv[1]

    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate"),
        autoescape=False,
        keep_trailing_newline=True,
    )

    template = env.get_template("GenericNumber.java.jinja")
    rootPath = f"{cmake_binary_dir}/generated/main/java/edu/wpi/first/math/numbers"

    for i in range(MAX_NUM + 1):
        contents = template.render(num=i)
        output(rootPath, f"N{i}.java", contents)

    template = env.get_template("Nat.java.jinja")
    rootPath = f"{cmake_binary_dir}/generated/main/java/edu/wpi/first/math"
    contents = template.render(nums=range(MAX_NUM + 1))
    output(rootPath, "Nat.java", contents)


if __name__ == "__main__":
    main()
