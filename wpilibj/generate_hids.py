#!/usr/bin/env python3

import json
import os

from jinja2 import Environment, FileSystemLoader


def Output(outPath, controllerName, contents):
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

    with open(f"{dirname}/src/generate/hids.json") as f:
        controllers = json.load(f)

    # Java files
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/"),
        autoescape=False,
        keep_trailing_newline=True,
    )
    rootPath = f"{dirname}/src/generated/main/java/edu/wpi/first/wpilibj"
    template = env.get_template("hid.java.jinja")
    for controller in controllers:
        controllerName = os.path.basename(f"{controller['ConsoleName']}Controller.java")
        output = template.render(controller)
        Output(rootPath, controllerName, output)

    # Java simulation files
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/"),
        autoescape=False,
        keep_trailing_newline=True,
    )
    rootPath = f"{dirname}/src/generated/main/java/edu/wpi/first/wpilibj/simulation"
    template = env.get_template("hidsim.java.jinja")
    for controller in controllers:
        controllerName = os.path.basename(
            f"{controller['ConsoleName']}ControllerSim.java"
        )
        output = template.render(controller)
        Output(rootPath, controllerName, output)


if __name__ == "__main__":
    main()
