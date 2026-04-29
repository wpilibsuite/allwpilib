import sys
import tomli

CONFIGS = {
    "examples": ("robotpyExamples/examples.toml", "robotpyExamples/example_projects.bzl", "PROJECTS"),
    "snippets": ("robotpyExamples/snippets.toml", "robotpyExamples/snippet_projects.bzl", "SNIPPETS"),
}

def generate(toml_filename, output_file, var_name):
    with open(toml_filename, "rb") as f:
        data = tomli.load(f)

    contents = var_name + " = [\n"
    for test_folder in data["tests"]["base"]:
        contents += f'    "{test_folder}",\n'
    contents += "]\n"

    with open(output_file, "w") as f:
        f.write(contents)

def main():
    if len(sys.argv) >= 2 and sys.argv[1] in CONFIGS:
        kind = sys.argv[1]
        toml_filename, default_output, var_name = CONFIGS[kind]
        output_file = sys.argv[2] if len(sys.argv) == 3 else default_output
        generate(toml_filename, output_file, var_name)
    else:
        # Legacy: single output file argument for examples
        toml_filename, default_output, var_name = CONFIGS["examples"]
        output_file = sys.argv[1] if len(sys.argv) == 2 else default_output
        generate(toml_filename, output_file, var_name)

if __name__ == "__main__":
    main()
