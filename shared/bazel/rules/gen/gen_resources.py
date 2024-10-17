import os
import argparse
import binascii


def generate_file(resource_file, output_file, prefix, namespace):
    func_name = "GetResource_" + os.path.basename(resource_file).replace(
        "-", "_"
    ).replace(".", "_")

    with open(resource_file, "rb") as f:
        raw_data = f.read()
        hex = str(binascii.hexlify(raw_data), "ascii")
        data = ", ".join("0x" + hex[i : i + 2] for i in range(0, len(hex), 2))
        data_size = len(raw_data)

    output = """#include <stddef.h>
#include <string_view>
extern "C" {{
static const unsigned char contents[] = {{ {data} }};
const unsigned char* {prefix}_{func_name}(size_t* len) {{
  *len = {data_size};
  return contents;
}}
}}
namespace {namespace} {{
std::string_view {func_name}() {{
  return std::string_view(reinterpret_cast<const char*>(contents), {data_size});
}}
}}
""".format(
        func_name=func_name,
        data_size=data_size,
        prefix=prefix,
        data=data,
        namespace=namespace,
    )

    with open(output_file, "w") as f:
        f.write(output)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--namespace")
    parser.add_argument("--prefix")
    parser.add_argument("--resources", nargs="+")
    parser.add_argument("--output_files", nargs="+")

    args = parser.parse_args()
    assert len(args.resources) == len(args.output_files)

    for i, resource in enumerate(args.resources):
        generate_file(resource, args.output_files[i], args.prefix, args.namespace)


if __name__ == "__main__":
    main()
