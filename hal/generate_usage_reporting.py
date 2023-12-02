import os


def main():
    java_package = "edu/wpi/first/hal"
    os.makedirs("hal/src/generated/main/native/include", exist_ok=True)
    os.makedirs(f"hal/src/generated/main/java/{java_package}", exist_ok=True)
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
        if os.path.exists(f"hal/src/generated/main/java/{java_package}/FRCNetComm.java"):
            with open(f"hal/src/generated/main/java/{java_package}/FRCNetComm.java", "w") as java_out:
                java_out.write(contents.replace(r"${usage_reporting_instances}", usage_reporting_instances))
        else:
            with open(f"hal/src/generated/main/java/{java_package}/FRCNetComm.java", "x") as java_out:
                java_out.write(contents.replace(r"${usage_reporting_instances}", usage_reporting_instances))

    with open("hal/src/generate/FRCUsageReporting.h.in") as cpp_usage_reporting:
        contents = cpp_usage_reporting.read()
        contents = contents.replace(r"${usage_reporting_types_cpp}",usage_reporting_types_cpp).replace(
            r"${usage_reporting_instances_cpp}", usage_reporting_instances_cpp)
        if os.path.exists("hal/src/generated/main/native/include/FRCUsageReporting.h"):
            with open("hal/src/generated/main/native/include/FRCUsageReporting.h", "w") as cpp_out:
                cpp_out.write(contents)
        else:
            with open("hal/src/generated/main/native/include/FRCUsageReporting.h", "x") as cpp_out:
                cpp_out.write(contents)


if __name__ == "__main__":
    main()
