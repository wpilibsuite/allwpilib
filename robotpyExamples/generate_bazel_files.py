
import sys
import tomli

def main():
    toml_filename = "robotpyExamples/examples.toml"
    output_file = "robotpyExamples/example_projects.bzl"
    
    with open(toml_filename, "rb") as f:
        data = tomli.load(f)

    contents = "PROJECTS = [\n"
    for test_folder in data["tests"]["base"]:
        contents += f'    "{test_folder}",\n'
    contents += "]\n"

    if len(sys.argv) == 2:
        output_file = sys.argv[1]
        
    with open(output_file, "w") as f:
        f.write(contents)


if __name__ == "__main__":
    main()
