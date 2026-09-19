#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import json
from pathlib import Path
from typing import Any

from jinja2 import Environment, FileSystemLoader
from jinja2.environment import Template


def render_template(
    template: Template, output_dir: Path, filename: str, entry_type: dict[str, Any]
):
    output_dir.mkdir(parents=True, exist_ok=True)
    (output_dir / filename).write_text(
        template.render(entry_type), encoding="utf-8", newline="\n"
    )


def generate_log_entries(output_root: Path, template_root: Path):
    with (template_root / "log_entry_types.json").open(encoding="utf-8") as f:
        entry_types = json.load(f)

    env = Environment(
        loader=FileSystemLoader(str(template_root / "main/java")),
        autoescape=False,
        keep_trailing_newline=True,
    )
    template = env.get_template("LogEntry.java.jinja")
    output_dir = output_root / "main/java/org/wpilib/datalog"

    for entry_type in entry_types:
        render_template(
            template, output_dir, f"{entry_type['Name']}LogEntry.java", entry_type
        )


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        help="Optional output directory for generated files",
        default=dirname / "src/generated",
        type=Path,
    )
    parser.add_argument(
        "--template_root",
        help="Optional root directory for the schema and Jinja templates",
        default=dirname / "src/generate",
        type=Path,
    )
    args = parser.parse_args()

    generate_log_entries(args.output_directory, args.template_root)


if __name__ == "__main__":
    main()
