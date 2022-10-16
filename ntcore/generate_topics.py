import glob
import os
import sys
from jinja2 import Environment, FileSystemLoader
import json
import argparse


GENERATED_FILES = []


def Output(outPath, outfn, contents):
    if not os.path.exists(outPath):
        os.makedirs(outPath)

    outpathname = f"{outPath}/{outfn}"
    GENERATED_FILES.append(outpathname)

    if os.path.exists(outpathname):
        with open(outpathname, "r") as f:
            if f.read() == contents:
                return

    # File either doesn't exist or has different contents
    with open(outpathname, "w") as f:
        f.write(contents)


def main():

    parser = argparse.ArgumentParser()
    parser.add_argument("--generate_java", action="store_true")
    parser.add_argument("--generate_cpp_includes", action="store_true")
    parser.add_argument("--generate_cpp_srcs", action="store_true")
    parser.add_argument("--generate_jni", action="store_true")

    # Is bazel
    is_bazel = "RUNFILES_MANIFEST_FILE" in os.environ or "RUNFILES_DIR" in os.environ

    if is_bazel:
        parser.add_argument("--output_files", nargs="+")
    else:
        parser.add_argument("--output_base")

    args = parser.parse_args()

    if is_bazel:
        dirname = "ntcore"

        files_to_generate = args.output_files
        base_name = os.path.dirname(files_to_generate[0])
        generation_root = "/".join(base_name.split("/")[0:4]) + "/generated"
    else:
        dirname, _ = os.path.split(os.path.abspath(__file__))
        generation_root = f"{args.output_base}/generated/main"

    with open(f"{dirname}/src/generate/types.json") as f:
        types = json.load(f)

    # Java files
    if args.generate_java:
        generate_java_files(dirname, generation_root, types)

    if args.generate_cpp_includes:
        generate_cpp_includes(dirname, generation_root, types)

    if args.generate_cpp_srcs:
        generate_cpp_srcs(dirname, generation_root, types)

    if args.generate_jni:
        generate_jni_files(dirname, generation_root, types)

    if is_bazel:
        diff = set(GENERATED_FILES).difference(files_to_generate)
        if len(diff) != 0:
            raise Exception(
                "File generated that was not listed in bazel:\n  "
                + "\n  ".join(x for x in diff)
            )


def generate_java_files(dirname, generation_root, types):
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/java"), autoescape=False
    )
    rootPath = f"{generation_root}/java/edu/wpi/first/networktables"
    for fn in glob.glob(f"{dirname}/src/generate/java/*.jinja"):
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


def generate_cpp_includes(dirname, generation_root, types):

    # C++ classes
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/include/networktables"),
        autoescape=False,
    )
    rootPath = f"{generation_root}/native/include/networktables"
    for fn in glob.glob(f"{dirname}/src/generate/include/networktables/*.jinja"):
        template = env.get_template(os.path.basename(fn))
        outfn = os.path.basename(fn)[:-6]  # drop ".jinja"
        for replacements in types:
            output = template.render(replacements)
            outfn2 = f"{replacements['TypeName']}{outfn}"
            Output(rootPath, outfn2, output)

    # C++ handle API (header)
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/include"), autoescape=False
    )
    template = env.get_template("ntcore_cpp_types.h.jinja")
    output = template.render(types=types)
    Output(
        f"{generation_root}/native/include",
        "ntcore_cpp_types.h",
        output,
    )

    # C handle API (header)
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/include"), autoescape=False
    )
    template = env.get_template("ntcore_c_types.h.jinja")
    output = template.render(types=types)
    Output(
        f"{generation_root}/native/include",
        "ntcore_c_types.h",
        output,
    )


def generate_cpp_srcs(dirname, generation_root, types):

    # C++ handle API (source)
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/cpp"), autoescape=False
    )
    template = env.get_template("ntcore_cpp_types.cpp.jinja")
    output = template.render(types=types)
    Output(f"{generation_root}/native/cpp", "ntcore_cpp_types.cpp", output)

    # C handle API (source)
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/cpp"), autoescape=False
    )
    template = env.get_template("ntcore_c_types.cpp.jinja")
    output = template.render(types=types)
    Output(f"{generation_root}/native/cpp", "ntcore_c_types.cpp", output)


def generate_jni_files(dirname, generation_root, types):

    # JNI
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/cpp/jni"), autoescape=False
    )
    template = env.get_template("types_jni.cpp.jinja")
    output = template.render(types=types)
    Output(f"{generation_root}/native/cpp/jni", "types_jni.cpp", output)


if __name__ == "__main__":
    main()
