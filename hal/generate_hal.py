import argparse
import sys
from pathlib import Path

from hal.generate_nanopb import generate_nanopb
from hal.generate_usage_reporting import generate_usage_reporting


def main(argv):
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
        "--template_root",
        help="Optional. If set, will use this directory as the root for the jinja templates",
        default=dirname / "src/generate",
        type=Path,
    )
    parser.add_argument(
        "--nanopb_generator",
        help="Path to the quickbuf protoc plugin",
        required=True,
    )
    parser.add_argument(
        "--proto_directory",
        help="Optional. If set, will use this directory to glob for protobuf files",
        default=dirname / "src/mrc/proto",
        type=Path,
    )
    args = parser.parse_args()

    print(args)

    generate_usage_reporting(args.output_directory, args.template_root)
    generate_nanopb(
        args.nanopb_generator,
        args.output_directory / "main/native/cpp/mrc/protobuf",
        args.proto_directory,
    )


if __name__ == "__main__":
    main(sys.argv[1:])
