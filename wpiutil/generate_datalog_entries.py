#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
import argparse
import json
import sys
from pathlib import Path
from typing import Any, Dict

from jinja2 import Environment, FileSystemLoader
from jinja2.environment import Template


def render_template(
    template: Template, output_dir: Path, filename: str, ty: Dict[str, Any]
):
    output_dir.mkdir(parents=True, exist_ok=True)

    output_file = output_dir / filename
    output_file.write_text(template.render(ty), encoding="utf-8")


def generate_log_entries(output_root, template_root):
    with (template_root / "types.json").open(encoding="utf-8") as f:
        types = json.load(f)["types"]

    env = Environment(
        loader=FileSystemLoader(str(template_root)),
        autoescape=False,
        keep_trailing_newline=True,
    )

    java_root_path = Path(output_root) / "java/edu/wpi/first/util/datalog"
    java_template = env.get_template("LogEntry.java.jinja")

    for ty in types:
        entry_name = f"{ty['name']}LogEntry.java"
        render_template(java_template, java_root_path, entry_name, ty)


def main(argv):
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        help="Optional. If set, will output the generated files to this directory, otherwise it will use a path relative to the script",
        default=dirname / "src/main/generated",
        type=Path,
    )
    parser.add_argument(
        "--template_root",
        help="Optional. If set, will use this directory as the root for the jinja templates",
        default=dirname / "src/main/generate",
        type=Path,
    )
    args = parser.parse_args(argv)

    generate_log_entries(args.output_directory, args.template_root)


if __name__ == "__main__":
    main(sys.argv[1:])
