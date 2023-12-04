#!/usr/bin/env python3

import glob
import os
import sys
import argparse
from jinja2 import Environment, FileSystemLoader
import json


def Output(outPath, outfn, contents):
    if not os.path.exists(outPath):
        os.makedirs(outPath)

    outpathname = f"{outPath}/{outfn}"

    # File either doesn't exist or has different contents
    with open(outpathname, "w", newline="\n") as f:
        f.write(contents)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        help="Optional. If set, will output the generated files to this directory, otherwise it will use a path relative to the script",
    )
    args = parser.parse_args()

    if args.output_directory:
        generation_root = args.output_directory
        template_root = "ntcore/src/generate"
    else:
        dirname, _ = os.path.split(os.path.abspath(__file__))
        generation_root = os.path.join(dirname, "src", "generated")
        template_root = os.path.join(dirname, "src", "generate")

    with open(f"{template_root}/types.json") as f:
        types = json.load(f)

    # Java files
    env = Environment(
        loader=FileSystemLoader(f"{template_root}/main/java"), autoescape=False
    )
    rootPath = f"{generation_root}/main/java/edu/wpi/first/networktables"
    for fn in glob.glob(f"{template_root}/main/java/*.jinja"):
        template = env.get_template(os.path.basename(fn))
        outfn = os.path.basename(fn)[:-6]  # drop ".jinja"
        if os.path.basename(fn).startswith("NetworkTable") or os.path.basename(
            fn
        ).startswith("Generic"):
            output = template.render(types=types)
            Output(rootPath, outfn, output)
        else:
            for replacements in types:
                output = template.render(replacements)
                if outfn == "Timestamped.java":
                    outfn2 = f"Timestamped{replacements['TypeName']}.java"
                else:
                    outfn2 = f"{replacements['TypeName']}{outfn}"
                Output(rootPath, outfn2, output)

    # C++ classes
    env = Environment(
        loader=FileSystemLoader(f"{template_root}/main/native/include/networktables"),
        autoescape=False,
    )
    rootPath = f"{generation_root}/main/native/include/networktables"
    for fn in glob.glob(f"{template_root}/main/native/include/networktables/*.jinja"):
        template = env.get_template(os.path.basename(fn))
        outfn = os.path.basename(fn)[:-6]  # drop ".jinja"
        for replacements in types:
            output = template.render(replacements)
            outfn2 = f"{replacements['TypeName']}{outfn}"
            Output(rootPath, outfn2, output)

    # C++ handle API (header)
    env = Environment(
        loader=FileSystemLoader(f"{template_root}/main/native/include"),
        autoescape=False,
    )
    template = env.get_template("ntcore_cpp_types.h.jinja")
    output = template.render(types=types)
    Output(
        f"{generation_root}/main/native/include",
        "ntcore_cpp_types.h",
        output,
    )

    # C++ handle API (source)
    env = Environment(
        loader=FileSystemLoader(f"{template_root}/main/native/cpp"),
        autoescape=False,
    )
    template = env.get_template("ntcore_cpp_types.cpp.jinja")
    output = template.render(types=types)
    Output(f"{generation_root}/main/native/cpp", "ntcore_cpp_types.cpp", output)

    # C handle API (header)
    env = Environment(
        loader=FileSystemLoader(f"{template_root}/main/native/include"),
        autoescape=False,
    )
    template = env.get_template("ntcore_c_types.h.jinja")
    output = template.render(types=types)
    Output(
        f"{generation_root}/main/native/include",
        "ntcore_c_types.h",
        output,
    )

    # C handle API (source)
    env = Environment(
        loader=FileSystemLoader(f"{template_root}/main/native/cpp"),
        autoescape=False,
    )
    template = env.get_template("ntcore_c_types.cpp.jinja")
    output = template.render(types=types)
    Output(f"{generation_root}/main/native/cpp", "ntcore_c_types.cpp", output)

    # JNI
    env = Environment(
        loader=FileSystemLoader(f"{template_root}/main/native/cpp/jni"),
        autoescape=False,
    )
    template = env.get_template("types_jni.cpp.jinja")
    output = template.render(types=types)
    Output(f"{generation_root}/main/native/cpp/jni", "types_jni.cpp", output)


if __name__ == "__main__":
    main()
