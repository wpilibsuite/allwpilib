import json
import sys


def load_foldernames(filename):
    output = []

    with open(filename, "r") as f:
        data = json.load(f)

    for test_data in data:
        output.append(test_data["foldername"])

    return sorted(set(output))


def load_tests(filename):
    output = []

    with open(filename, "r") as f:
        data = json.load(f)

    for test_data in data:
        if test_data.get("hasunittests", False):
            output.append(test_data["foldername"])

    return sorted(set(output))


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
    tests = load_tests(
        "wpilibjExamples/src/main/java/org/wpilib/examples/examples.json"
    )

    output_file = "wpilibjExamples/example_projects.bzl"
    if len(sys.argv) == 2:
        output_file = sys.argv[1]

    with open(output_file, "w") as f:
        f.write(
            'EXAMPLES_FOLDERS = [\n    "' + '",\n    "'.join(examples) + '",\n]\n\n'
        )
        f.write(
            'COMMANDS_V2_FOLDERS = [\n    "' + '",\n    "'.join(commands) + '",\n]\n\n'
        )
        f.write(
            'SNIPPETS_FOLDERS = [\n    "' + '",\n    "'.join(snippets) + '",\n]\n\n'
        )
        f.write(
            'TEMPLATES_FOLDERS = [\n    "' + '",\n    "'.join(templates) + '",\n]\n\n'
        )
        f.write('TEST_FOLDERS = [\n    "' + '",\n    "'.join(tests) + '",\n]\n')


if __name__ == "__main__":
    main()
