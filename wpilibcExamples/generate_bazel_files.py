import json
import sys


def load_foldernames(filename):
    output = []

    with open(filename, "r") as f:
        data = json.load(f)

    for test_data in data:
        vendordeps = []
        dynamic_deps = []

        for extra_vendordep in test_data.get("extravendordeps", []):
            if extra_vendordep == "cameraserver":
                vendordeps.append("//cameraserver")
                dynamic_deps.append("//cameraserver:shared/cameraserver")
                dynamic_deps.append("//cscore:shared/cscore")

                # Opencv is special, so we'll just add a special case we can handle when writing
                dynamic_deps.append("opencv_placeholder")
            elif extra_vendordep == "romi":
                vendordeps.append("//romiVendordep")
                dynamic_deps.append("//romiVendordep:shared/romiVendordep")
            elif extra_vendordep == "xrp":
                vendordeps.append("//xrpVendordep")
                dynamic_deps.append("//xrpVendordep:shared/xrpVendordep")
            else:
                raise Exception(f"Unknown vendordep mapping for '{extra_vendordep}'")

        command_version = test_data.get("commandversion", None)
        if command_version == 2:
            vendordeps.append("//commandsv2")
            dynamic_deps.append("//commandsv2:shared/commandsv2")
        output.append(
            (test_data["foldername"], sorted(vendordeps), sorted(dynamic_deps))
        )

    return sorted(output)


def load_tests(filename):
    output = []

    with open(filename, "r") as f:
        data = json.load(f)

    for test_data in data:
        if test_data.get("hasunittests", False):
            output.append((test_data["foldername"], [], []))

    return sorted(output)


def dump_chunk(constant_name, chunk_list):
    output = constant_name + " = {\n"
    for project_name, vendordeps, dynamic_deps in chunk_list:
        dynamic_deps_txt = ""
        if "opencv_placeholder" in dynamic_deps:
            dynamic_deps.remove("opencv_placeholder")
            dynamic_deps_txt += "opencv_shared_libraries + "
        dynamic_deps_txt += f"[{', '.join(f'"{x}"' for x in dynamic_deps)}]"

        output += f'    "{project_name}": ([{", ".join(f'"{x}"' for x in vendordeps)}], {dynamic_deps_txt}),\n'
    output += "}\n"
    return output


def main():
    examples = load_foldernames("wpilibcExamples/src/main/cpp/examples/examples.json")
    commands = load_foldernames("wpilibcExamples/src/main/cpp/commands/commands.json")
    templates = load_foldernames(
        "wpilibcExamples/src/main/cpp/templates/templates.json"
    )
    snippets = load_foldernames("wpilibcExamples/src/main/cpp/snippets/snippets.json")
    example_tests = load_tests("wpilibcExamples/src/main/cpp/examples/examples.json")
    snippet_tests = load_tests("wpilibcExamples/src/main/cpp/snippets/snippets.json")

    output_file = "wpilibcExamples/example_projects.bzl"
    if len(sys.argv) == 2:
        output_file = sys.argv[1]

    with open(output_file, "w", encoding="utf-8", newline="\n") as f:
        f.write(
            'load("@bzlmodrio-opencv//libraries/cpp/opencv:libraries.bzl", "opencv_shared_libraries")\n\n'
        )
        f.write(dump_chunk("EXAMPLE_FOLDERS", examples) + "\n")
        f.write(dump_chunk("COMMANDS_V2_FOLDERS", commands) + "\n")
        f.write(dump_chunk("SNIPPET_FOLDERS", snippets) + "\n")
        f.write(dump_chunk("TEMPLATE_FOLDERS", templates) + "\n")

        f.write(dump_chunk("EXAMPLE_TESTS_FOLDERS", example_tests) + "\n")
        f.write(dump_chunk("SNIPPET_TESTS_FOLDERS", snippet_tests))


if __name__ == "__main__":
    main()
