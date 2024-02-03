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

    # C++ headers
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/main/native/include/frc"),
        autoescape=False,
        keep_trailing_newline=True,
    )
    rootPath = f"{dirname}/src/generated/main/native/include/frc"
    template = env.get_template("hid.h.jinja")
    for controller in controllers:
        controllerName = os.path.basename(f"{controller['ConsoleName']}Controller.h")
        output = template.render(controller)
        write_controller_file(rootPath, controllerName, output)

    # C++ files
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/main/native/cpp"),
        autoescape=False,
    )
    rootPath = f"{dirname}/src/generated/main/native/cpp"
    template = env.get_template("hid.cpp.jinja")
    for controller in controllers:
        controllerName = os.path.basename(f"{controller['ConsoleName']}Controller.cpp")
        output = template.render(controller)
        write_controller_file(rootPath, controllerName, output)

    # C++ simulation headers
    env = Environment(
        loader=FileSystemLoader(
            f"{dirname}/src/generate/main/native/include/frc/simulation"
        ),
        autoescape=False,
        keep_trailing_newline=True,
    )
    rootPath = f"{dirname}/src/generated/main/native/include/frc/simulation"
    template = env.get_template("hidsim.h.jinja")
    for controller in controllers:
        controllerName = os.path.basename(f"{controller['ConsoleName']}ControllerSim.h")
        output = template.render(controller)
        write_controller_file(rootPath, controllerName, output)

    # C++ simulation files
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/main/native/cpp/simulation"),
        autoescape=False,
    )
    rootPath = f"{dirname}/src/generated/main/native/cpp/simulation"
    template = env.get_template("hidsim.cpp.jinja")
    for controller in controllers:
        controllerName = os.path.basename(
            f"{controller['ConsoleName']}ControllerSim.cpp"
        )
        output = template.render(controller)
        write_controller_file(rootPath, controllerName, output)


if __name__ == "__main__":
    main()
