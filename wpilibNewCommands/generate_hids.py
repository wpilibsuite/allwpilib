#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
import json
import os

from jinja2 import Environment, FileSystemLoader


def write_controller_file(outPath, controllerName, contents):
    if not os.path.exists(outPath):
        os.makedirs(outPath)

    outpathname = f"{outPath}/{controllerName}"

    if os.path.exists(outpathname):
        with open(outpathname, "r") as f:
            if f.read() == contents:
                return

    # File either doesn't exist or has different contents
    with open(outpathname, "w", newline="\n") as f:
        f.write(contents)


def main():
    dirname, _ = os.path.split(os.path.abspath(__file__))

    with open("wpilibj/src/generate/hids.json") as f:
        controllers = json.load(f)

    # Java files
    env = Environment(
        loader=FileSystemLoader(
            f"{dirname}/src/generate/main/java/edu/wpi/first/wpilibj2/command/button"
        ),
        autoescape=False,
        keep_trailing_newline=True,
    )
    rootPath = (
        f"{dirname}/src/generated/main/java/edu/wpi/first/wpilibj2/command/button"
    )
    template = env.get_template("commandhid.java.jinja")
    for controller in controllers:
        controllerName = os.path.basename(
            f"Command{controller['ConsoleName']}Controller.java"
        )
        output = template.render(controller)
        write_controller_file(rootPath, controllerName, output)

    # C++ headers
    env = Environment(
        loader=FileSystemLoader(
            f"{dirname}/src/generate/main/native/include/frc2/command/button"
        ),
        autoescape=False,
        keep_trailing_newline=True,
    )
    rootPath = f"{dirname}/src/generated/main/native/include/frc2/command/button"
    template = env.get_template("commandhid.h.jinja")
    for controller in controllers:
        controllerName = os.path.basename(
            f"Command{controller['ConsoleName']}Controller.h"
        )
        output = template.render(controller)
        write_controller_file(rootPath, controllerName, output)

    # C++ files
    env = Environment(
        loader=FileSystemLoader(
            f"{dirname}/src/generate/main/native/cpp/frc2/command/button"
        ),
        autoescape=False,
    )
    rootPath = f"{dirname}/src/generated/main/native/cpp/frc2/command/button"
    template = env.get_template("commandhid.cpp.jinja")
    for controller in controllers:
        controllerName = os.path.basename(
            f"Command{controller['ConsoleName']}Controller.cpp"
        )
        output = template.render(controller)
        write_controller_file(rootPath, controllerName, output)


if __name__ == "__main__":
    main()
