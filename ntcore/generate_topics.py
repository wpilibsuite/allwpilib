import glob
import os
import sys
from jinja2 import Environment, FileSystemLoader
import json


def Output(outPath, outfn, contents):
    if not os.path.exists(outPath):
        os.makedirs(outPath)

    outpathname = f"{outPath}/{outfn}"

    if os.path.exists(outpathname):
        with open(outpathname, "r") as f:
            if f.read() == contents:
                return

    # File either doesn't exist or has different contents
    with open(outpathname, "w") as f:
        f.write(contents)


def main():
    dirname, _ = os.path.split(os.path.abspath(__file__))
    cmake_binary_dir = sys.argv[1]

    with open(f"{dirname}/src/generate/types.json") as f:
        types = json.load(f)

    # Java files
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/java"), autoescape=False
    )
    rootPath = f"{cmake_binary_dir}/generated/main/java/edu/wpi/first/networktables"
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

    # C++ classes
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/include/networktables"),
        autoescape=False,
    )
    rootPath = f"{cmake_binary_dir}/generated/main/native/include/networktables"
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
        f"{cmake_binary_dir}/generated/main/native/include",
        "ntcore_cpp_types.h",
        output,
    )

    # C++ handle API (source)
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/cpp"), autoescape=False
    )
    template = env.get_template("ntcore_cpp_types.cpp.jinja")
    output = template.render(types=types)
    Output(
        f"{cmake_binary_dir}/generated/main/native/cpp", "ntcore_cpp_types.cpp", output
    )

    # C handle API (header)
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/include"), autoescape=False
    )
    template = env.get_template("ntcore_c_types.h.jinja")
    output = template.render(types=types)
    Output(
        f"{cmake_binary_dir}/generated/main/native/include",
        "ntcore_c_types.h",
        output,
    )

    # C handle API (source)
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/cpp"), autoescape=False
    )
    template = env.get_template("ntcore_c_types.cpp.jinja")
    output = template.render(types=types)
    Output(
        f"{cmake_binary_dir}/generated/main/native/cpp", "ntcore_c_types.cpp", output
    )

    # JNI
    env = Environment(
        loader=FileSystemLoader(f"{dirname}/src/generate/cpp/jni"), autoescape=False
    )
    template = env.get_template("types_jni.cpp.jinja")
    output = template.render(types=types)
    Output(f"{cmake_binary_dir}/generated/main/native/cpp/jni", "types_jni.cpp", output)


if __name__ == "__main__":
    main()
