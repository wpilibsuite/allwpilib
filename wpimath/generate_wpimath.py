import sys
from pathlib import Path

# When invoked directly, Python puts the script directory on sys.path.
# Add the repo root so absolute package imports still work.
sys.path.insert(0, str(Path(__file__).absolute().parent.parent))

from shared.generation import (
    GeneratorTypes,
    add_jinja_args,
    generate_python_protobuf,
    make_arg_parser,
)
from wpimath.generate_nanopb import generate_nanopb
from wpimath.generate_numbers import generate_numbers
from wpimath.generate_quickbuf import generate_quickbuf


def main(argv):
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = make_arg_parser(
        dirname, dirname.parent, GeneratorTypes.NANOPB | GeneratorTypes.QUICKBUF
    )
    add_jinja_args(parser, dirname, None)
    parser.add_argument(
        "--python_output_directory",
        help="Where to write the generated Python protobuf modules. Pass __none__ to skip them.",
        default=dirname / "src/main/python/wpimath/protobuf/_generated",
        type=Path,
    )
    args = parser.parse_args(argv)

    generate_numbers(args.output_directory, args.template_root)
    generate_nanopb(
        args.nanopb,
        args.output_directory / "main/native/cpp",
        args.output_directory / "main/native/include",
        args.proto_directory,
    )
    generate_quickbuf(
        args.protoc,
        args.quickbuf_plugin,
        args.output_directory / "main/java",
        args.proto_directory,
    )
    if args.python_output_directory.name != "__none__":
        # Use grpcio-tools' protoc rather than args.protoc: the generated code must match
        # the pinned protobuf runtime, which args.protoc (used for C++/Java) does not.
        generate_python_protobuf(
            [sys.executable, "-m", "grpc_tools.protoc"],
            args.python_output_directory,
            args.proto_directory,
        )


if __name__ == "__main__":
    main(sys.argv[1:])
