#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
from pathlib import Path


def generate_usage_reporting(output_directory: Path, template_directory: Path):
    # Gets the folder this script is in (the hal/ directory)
    java_package = "edu/wpi/first/hal"
    # fmt: off
    (output_directory / "main/native/include/hal").mkdir(parents=True, exist_ok=True)
    (output_directory / f"main/java/{java_package}").mkdir(parents=True, exist_ok=True)
    # fmt: on
    usage_reporting_types_cpp = []
    usage_reporting_instances_cpp = []
    usage_reporting_types = []
    usage_reporting_instances = []
    with (template_directory / "Instances.txt").open(encoding="utf-8") as instances:
        for instance in instances:
            usage_reporting_instances_cpp.append(f"    {instance.strip()},")
            usage_reporting_instances.append(
                f"    /** {instance.strip()}. */\n"
                f"    public static final int {instance.strip()};"
            )

    with (template_directory / "ResourceType.txt").open(
        encoding="utf-8"
    ) as resource_types:
        for resource_type in resource_types:
            usage_reporting_types_cpp.append(f"    {resource_type.strip()},")
            usage_reporting_types.append(
                f"    /** {resource_type.strip()}. */\n"
                f"    public static final int {resource_type.strip()};"
            )

    with (template_directory / "FRCNetComm.java.in").open(
        encoding="utf-8"
    ) as java_usage_reporting:
        contents = (
            # fmt: off
            java_usage_reporting.read()
            .replace(r"${usage_reporting_types}", "\n".join(usage_reporting_types))
            .replace(r"${usage_reporting_instances}", "\n".join(usage_reporting_instances))
            # fmt: on
        )

        frc_net_comm = output_directory / f"main/java/{java_package}/FRCNetComm.java"
        frc_net_comm.write_text(contents, encoding="utf-8", newline="\n")

    with (template_directory / "FRCUsageReporting.h.in").open(
        encoding="utf-8"
    ) as cpp_usage_reporting:
        contents = (
            # fmt: off
            cpp_usage_reporting.read()
            .replace(r"${usage_reporting_types_cpp}", "\n".join(usage_reporting_types_cpp))
            .replace(r"${usage_reporting_instances_cpp}", "\n".join(usage_reporting_instances_cpp))
            # fmt: on
        )

        usage_reporting_hdr = (
            output_directory / "main/native/include/hal/FRCUsageReporting.h"
        )
        usage_reporting_hdr.write_text(contents, encoding="utf-8", newline="\n")

    with (template_directory / "UsageReporting.h.in").open(
        encoding="utf-8"
    ) as cpp_usage_reporting:
        contents = (
            # fmt: off
            cpp_usage_reporting.read()
            .replace(r"${usage_reporting_types_cpp}", "\n".join(usage_reporting_types_cpp))
            .replace(r"${usage_reporting_instances_cpp}", "\n".join(usage_reporting_instances_cpp))
            # fmt: on
        )

        usage_reporting_hdr = (
            output_directory / "main/native/include/hal/UsageReporting.h"
        )
        usage_reporting_hdr.write_text(contents, encoding="utf-8", newline="\n")


def main():
    dirname = Path(__file__).parent

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        help="Optional. If set, will output the generated files to this directory, otherwise it will use a path relative to the script",
        default=dirname / "src/generated",
        type=Path,
    )
    parser.add_argument(
        "--template_root",
        help="Optional. If set, will use this directory as the root for the jinja templates",
        default=dirname / "src/generate",
        type=Path,
    )
    args = parser.parse_args()

    generate_usage_reporting(args.output_directory, args.template_root)


if __name__ == "__main__":
    main()
