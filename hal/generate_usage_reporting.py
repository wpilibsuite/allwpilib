import os


def main():
    os.makedirs("hal/src/generated/main/native/include", exist_ok=True)
    os.makedirs("hal/src/generated/main/native/java", exist_ok=True)
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
        contents = contents.replace(r"${usage_reporting_types}", usage_reporting_types)
        with open("hal/src/generated/main/native/java/FRCNetComm.java", "w") as java_out:
            java_out.write(contents.replace(r"${usage_reporting_instances}", usage_reporting_instances))

    with open("hal/src/generate/FRCUsageReporting.h.in") as cpp_usage_reporting:
        contents = cpp_usage_reporting.read()
        contents = contents.replace(r"${usage_reporting_types_cpp}",
                         usage_reporting_types_cpp)
        with open("hal/src/generated/main/native/include/FRCUsageReporting.h", "w") as cpp_out:
            cpp_out.write(contents.replace(
                r"${usage_reporting_instances_cpp}", usage_reporting_instances_cpp))


if __name__ == "__main__":
    main()
