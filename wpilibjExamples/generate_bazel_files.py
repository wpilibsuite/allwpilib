import json
import sys


def load_foldernames(filename):
    output = []

    with open(filename, "r") as f:
        data = json.load(f)

    for test_data in data:
        vendordeps = []

        for extra_vendordep in test_data.get("extravendordeps", []):
            if extra_vendordep == "cameraserver":
                vendordeps.append("//cameraserver:cameraserver-java")
                vendordeps.append("//cscore:cscore-java")
            elif extra_vendordep == "romi":
                vendordeps.append("//romiVendordep:romiVendordep-java")
            elif extra_vendordep == "xrp":
                vendordeps.append("//xrpVendordep:xrpVendordep-java")
            else:
                raise Exception(f"Unknown vendordep mapping for '{extra_vendordep}'")

        command_version = test_data.get("commandversion", None)
        if command_version == 2:
            vendordeps.append("//commandsv2:commandsv2-java")
        elif command_version == 3:
            vendordeps.append("//commandsv3:commandsv3-java")
        output.append((test_data["foldername"], sorted(vendordeps)))

    return sorted(output)


def load_tests(filename):
    output = []

    with open(filename, "r") as f:
        data = json.load(f)

    for test_data in data:
        if test_data.get("hasunittests", False):
            output.append((test_data["foldername"], []))

    return sorted(output)


def dump_chunk(constant_name, chunk_list):
    output = constant_name + " = {\n"
    for project_name, vendordeps in chunk_list:
        output += (
            f'    "{project_name}": [{", ".join(f'"{x}"' for x in vendordeps)}],\n'
        )
    output += "}\n"
    return output


def main():
    examples = load_foldernames(
        "wpilibjExamples/src/main/java/org/wpilib/examples/examples.json"
    )
    commands = load_foldernames(
        "wpilibjExamples/src/main/java/org/wpilib/commands/commands.json"
    )
    templates = load_foldernames(
        "wpilibjExamples/src/main/java/org/wpilib/templates/templates.json"
    )
    snippets = load_foldernames(
        "wpilibjExamples/src/main/java/org/wpilib/snippets/snippets.json"
    )
    example_tests = load_tests(
        "wpilibjExamples/src/main/java/org/wpilib/examples/examples.json"
    )
    snippet_tests = load_tests(
        "wpilibjExamples/src/main/java/org/wpilib/snippets/snippets.json"
    )

    output_file = "wpilibjExamples/example_projects.bzl"
    if len(sys.argv) == 2:
        output_file = sys.argv[1]

    with open(output_file, "w", encoding="utf-8", newline="\n") as f:
        f.write(dump_chunk("EXAMPLE_FOLDERS", examples) + "\n")
        f.write(dump_chunk("COMMANDS_V2_FOLDERS", commands) + "\n")
        f.write(dump_chunk("SNIPPET_FOLDERS", snippets) + "\n")
        f.write(dump_chunk("TEMPLATE_FOLDERS", templates) + "\n")

        f.write(dump_chunk("EXAMPLE_TESTS_FOLDERS", example_tests) + "\n")
        f.write(dump_chunk("SNIPPET_TESTS_FOLDERS", snippet_tests))


if __name__ == "__main__":
    main()
