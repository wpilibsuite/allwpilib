#!/usr/bin/env python3

import json
import sys
from pathlib import Path

# When invoked directly, Python puts the script directory on sys.path.
# Add the repo root so absolute package imports still work.
sys.path.insert(0, str(Path(__file__).absolute().parent.parent))

from jinja2 import Environment, FileSystemLoader
from shared.generation import add_jinja_args, make_arg_parser, write_file


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

    generated_output_dir = output_directory / "main/java/org/wpilib/networktables"
    for fn in java_template_directory.glob("*.jinja"):
        template = env.get_template(fn.name)
        outfn = fn.stem
        if outfn.startswith("NetworkTable") or outfn.startswith("Generic"):
            write_file(generated_output_dir, outfn, template.render(types=types))
        else:
            for replacements in types:
                if outfn == "Timestamped.java":
                    outfn2 = f"Timestamped{replacements['TypeName']}.java"
                else:
                    outfn2 = f"{replacements['TypeName']}{outfn}"
                write_file(generated_output_dir, outfn2, template.render(replacements))

    # C++ classes
    cpp_subdirectory = "main/native/include/wpi/nt"
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
            write_file(generated_output_dir, outfn2, output)

    # C++ handle API (header)
    hdr_subdirectory = "main/native/include"
    hdr_template_directory = template_root / hdr_subdirectory
    env = Environment(
        loader=FileSystemLoader(hdr_template_directory),
        autoescape=False,
    )
    template = env.get_template("ntcore_cpp_types.hpp.jinja")
    write_file(
        output_directory / hdr_subdirectory / "wpi/nt",
        "ntcore_cpp_types.hpp",
        template.render(types=types),
    )

    # C++ handle API (source)
    cpp_subdirectory = "main/native/cpp"
    cpp_template_directory = template_root / cpp_subdirectory
    env = Environment(
        loader=FileSystemLoader(cpp_template_directory),
        autoescape=False,
    )
    template = env.get_template("ntcore_cpp_types.cpp.jinja")
    write_file(
        output_directory / cpp_subdirectory,
        "ntcore_cpp_types.cpp",
        template.render(types=types),
    )

    # C handle API (header)
    hdr_subdirectory = "main/native/include"
    hdr_template_directory = template_root / hdr_subdirectory
    env = Environment(
        loader=FileSystemLoader(hdr_template_directory),
        autoescape=False,
    )
    template = env.get_template("ntcore_c_types.h.jinja")
    write_file(
        output_directory / hdr_subdirectory / "wpi/nt",
        "ntcore_c_types.h",
        template.render(types=types),
    )

    # C handle API (source)
    c_subdirectory = "main/native/cpp"
    c_template_directory = template_root / c_subdirectory
    env = Environment(
        loader=FileSystemLoader(c_template_directory),
        autoescape=False,
    )
    template = env.get_template("ntcore_c_types.cpp.jinja")
    write_file(
        output_directory / c_subdirectory,
        "ntcore_c_types.cpp",
        template.render(types=types),
    )

    # JNI
    jni_subdirectory = "main/native/cpp/jni"
    jni_template_directory = template_root / jni_subdirectory
    env = Environment(
        loader=FileSystemLoader(jni_template_directory),
        autoescape=False,
    )
    template = env.get_template("types_jni.cpp.jinja")
    write_file(
        output_directory / jni_subdirectory,
        "types_jni.cpp",
        template.render(types=types),
    )


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = make_arg_parser(dirname, dirname.parent)
    add_jinja_args(parser, dirname, dirname / "src/generate/types.json")
    args = parser.parse_args()

    generate_topics(args.output_directory, args.template_root, args.schema_file)


if __name__ == "__main__":
    main()
