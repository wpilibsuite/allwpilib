#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
import pathlib


def main():
    # Gets the folder this script is in (the hal/ directory)
    HAL_ROOT = pathlib.Path(__file__).parent
    java_package = "edu/wpi/first/hal"
    # fmt: off
    (HAL_ROOT / "src/generated/main/native/include/hal").mkdir(parents=True, exist_ok=True)
    (HAL_ROOT / f"src/generated/main/java/{java_package}").mkdir(parents=True, exist_ok=True)
    # fmt: on
    usage_reporting_types_cpp = []
    usage_reporting_instances_cpp = []
    usage_reporting_types = []
    usage_reporting_instances = []
    with open(HAL_ROOT / "src/generate/Instances.txt") as instances:
        for instance in instances:
            usage_reporting_instances_cpp.append(f"    {instance.strip()},")
            usage_reporting_instances.append(
                f"    /** {instance.strip()}. */\n"
                f"    public static final int {instance.strip()};"
            )

    with open(HAL_ROOT / "src/generate/ResourceType.txt") as resource_types:
        for resource_type in resource_types:
            usage_reporting_types_cpp.append(f"    {resource_type.strip()},")
            usage_reporting_types.append(
                f"    /** {resource_type.strip()}. */\n"
                f"    public static final int {resource_type.strip()};"
            )

    with open(HAL_ROOT / "src/generate/FRCNetComm.java.in") as java_usage_reporting:
        contents = (
            # fmt: off
            java_usage_reporting.read()
            .replace(r"${usage_reporting_types}", "\n".join(usage_reporting_types))
            .replace(r"${usage_reporting_instances}", "\n".join(usage_reporting_instances))
            # fmt: on
        )

        with open(
            HAL_ROOT / f"src/generated/main/java/{java_package}/FRCNetComm.java", "w"
        ) as java_out:
            java_out.write(contents)

    with open(HAL_ROOT / "src/generate/FRCUsageReporting.h.in") as cpp_usage_reporting:
        contents = (
            # fmt: off
            cpp_usage_reporting.read()
            .replace(r"${usage_reporting_types_cpp}", "\n".join(usage_reporting_types_cpp))
            .replace(r"${usage_reporting_instances_cpp}", "\n".join(usage_reporting_instances_cpp))
            # fmt: on
        )

        with open(
            HAL_ROOT / "src/generated/main/native/include/hal/FRCUsageReporting.h", "w"
        ) as cpp_out:
            cpp_out.write(contents)


if __name__ == "__main__":
    main()
