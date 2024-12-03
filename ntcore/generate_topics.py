#!/usr/bin/env python3

import argparse
import json
from pathlib import Path

from jinja2 import Environment, FileSystemLoader


def Output(output_dir: Path, controller_name: str, contents: str):
    output_dir.mkdir(parents=True, exist_ok=True)
    output_file = output_dir / controller_name
    output_file.write_text(contents, encoding="utf-8", newline="\n")


def generate_topics(
    output_directory: Path, template_root: Path, types_schema_file: Path
):
    with (types_schema_file).open(encoding="utf-8") as f:
        types = json.load(f)

    # Java files
    java_template_directory = template_root / "main/java"
    env = Environment(
        loader=FileSystemLoader(java_template_directory), autoescape=False
    )

    generated_output_dir = output_directory / "main/java/edu/wpi/first/networktables"
    for fn in java_template_directory.glob("*.jinja"):
        template = env.get_template(fn.name)
        outfn = fn.stem
        if outfn.startswith("NetworkTable") or outfn.startswith("Generic"):
            output = template.render(types=types)
            Output(generated_output_dir, outfn, output)
        else:
            for replacements in types:
                output = template.render(replacements)
                if outfn == "Timestamped.java":
                    outfn2 = f"Timestamped{replacements['TypeName']}.java"
                else:
                    outfn2 = f"{replacements['TypeName']}{outfn}"
                Output(generated_output_dir, outfn2, output)

    # C++ classes
    cpp_subdirectory = "main/native/include/networktables"
    cpp_template_directory = template_root / cpp_subdirectory
    env = Environment(
        loader=FileSystemLoader(cpp_template_directory),
        autoescape=False,
    )

    generated_output_dir = output_directory / cpp_subdirectory
    for fn in cpp_template_directory.glob("*.jinja"):
        template = env.get_template(fn.name)
        outfn = fn.stem  # drop ".jinja"
        for replacements in types:
            output = template.render(replacements)
            outfn2 = f"{replacements['TypeName']}{outfn}"
            Output(generated_output_dir, outfn2, output)

    # C++ handle API (header)
    hdr_subdirectory = "main/native/include"
    hdr_template_directory = template_root / hdr_subdirectory
    env = Environment(
        loader=FileSystemLoader(hdr_template_directory),
        autoescape=False,
    )
    template = env.get_template("ntcore_cpp_types.h.jinja")
    output = template.render(types=types)
    Output(
        output_directory / hdr_subdirectory,
        "ntcore_cpp_types.h",
        output,
    )

    # C++ handle API (source)
    cpp_subdirectory = "main/native/cpp"
    cpp_template_directory = template_root / cpp_subdirectory
    env = Environment(
        loader=FileSystemLoader(cpp_template_directory),
        autoescape=False,
    )
    template = env.get_template("ntcore_cpp_types.cpp.jinja")
    output = template.render(types=types)
    Output(
        output_directory / cpp_subdirectory,
        "ntcore_cpp_types.cpp",
        output,
    )

    # C handle API (header)
    hdr_subdirectory = "main/native/include"
    hdr_template_directory = template_root / hdr_subdirectory
    env = Environment(
        loader=FileSystemLoader(hdr_template_directory),
        autoescape=False,
    )
    template = env.get_template("ntcore_c_types.h.jinja")
    output = template.render(types=types)
    Output(output_directory / hdr_subdirectory, "ntcore_c_types.h", output)

    # C handle API (source)
    c_subdirectory = "main/native/cpp"
    c_template_directory = template_root / c_subdirectory
    env = Environment(
        loader=FileSystemLoader(c_template_directory),
        autoescape=False,
    )
    template = env.get_template("ntcore_c_types.cpp.jinja")
    output = template.render(types=types)
    Output(output_directory / c_subdirectory, "ntcore_c_types.cpp", output)

    # JNI
    jni_subdirectory = "main/native/cpp/jni"
    jni_template_directory = template_root / jni_subdirectory
    env = Environment(
        loader=FileSystemLoader(jni_template_directory),
        autoescape=False,
    )
    template = env.get_template("types_jni.cpp.jinja")
    output = template.render(types=types)
    Output(output_directory / jni_subdirectory, "types_jni.cpp", output)


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        help="Optional. If set, will output the generated files to this directory, otherwise it will use a path relative to the script",
        default=dirname / "src/generated",
        type=Path,
    )
    parser.add_argument(
        "--types_schema_file",
        help="Optional. If set, this file will be used to load the types schema",
        default=dirname / "src/generate/types.json",
        type=Path,
    )
    parser.add_argument(
        "--template_root",
        help="Optional. If set, will use this directory as the root for the jinja templates",
        default=dirname / "src/generate",
        type=Path,
    )
    args = parser.parse_args()

    generate_topics(args.output_directory, args.template_root, args.types_schema_file)


if __name__ == "__main__":
    main()
