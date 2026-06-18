import sys

import tomli


def load_project_names(toml_filename, project_type):

    with open(toml_filename, "rb") as f:
        data = tomli.load(f)

    contents = f"{project_type.upper()}_PROJECTS = [\n"
    for test_folder in data["tests"]["base"]:
        contents += f'    "{project_type}s/{test_folder}",\n'
    contents += "]\n"

    return contents


def main():
    output_file = "robotpyExamples/example_projects.bzl"

    contents = load_project_names("robotpyExamples/examples/examples.toml", "example")
    contents += load_project_names("robotpyExamples/snippets/snippets.toml", "snippet")

    if len(sys.argv) == 2:
        output_file = sys.argv[1]

    with open(output_file, "w") as f:
        f.write(contents)


if __name__ == "__main__":
    main()
