#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
import os
import argparse

def main():

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        help="Optional. If set, will output the generated files to this directory, otherwise it will use a path relative to the script",
    )
    args = parser.parse_args()

    output_dir = args.output_directory or "hal/src/generated/"

    java_package = "edu/wpi/first/hal"
    os.makedirs(f"{output_dir}/main/native/include/hal", exist_ok=True)
    os.makedirs(f"{output_dir}/main/java/{java_package}", exist_ok=True)
    usage_reporting_types_cpp = ""
    usage_reporting_instances_cpp = ""
    usage_reporting_types = ""
    usage_reporting_instances = ""
    with open("hal/src/generate/Instances.txt") as instances:
        for instance in instances:
            usage_reporting_instances_cpp = f"{usage_reporting_instances_cpp}\n    {instance.strip()},"
            usage_reporting_instances = f"{usage_reporting_instances}\n    public static final int {instance.strip()};"

    with open("hal/src/generate/ResourceType.txt") as resource_types:
        for resource_type in resource_types:
            usage_reporting_types_cpp = f"{usage_reporting_types_cpp}\n    {resource_type.strip()},"
            usage_reporting_types = f"{usage_reporting_types}\n    public static final int {resource_type.strip()};"

    with open("hal/src/generate/FRCNetComm.java.in") as java_usage_reporting:
        contents = java_usage_reporting.read()
        contents = contents.replace(
            r"${usage_reporting_types}", usage_reporting_types)
        with open(f"{output_dir}/main/java/{java_package}/FRCNetComm.java", "w") as java_out:
            java_out.write(contents.replace(
                r"${usage_reporting_instances}", usage_reporting_instances))

    with open("hal/src/generate/FRCUsageReporting.h.in") as cpp_usage_reporting:
        contents = cpp_usage_reporting.read()
        contents = contents.replace(r"${usage_reporting_types_cpp}", usage_reporting_types_cpp).replace(
            r"${usage_reporting_instances_cpp}", usage_reporting_instances_cpp)
        with open(f"{output_dir}/main/native/include/hal/FRCUsageReporting.h", "w") as cpp_out:
            cpp_out.write(contents)


if __name__ == "__main__":
    main()
