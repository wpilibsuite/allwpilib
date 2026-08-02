import sys
from pathlib import Path

# When invoked directly, Python puts the script directory on sys.path.
# Add the repo root so absolute package imports still work.
sys.path.insert(0, str(Path(__file__).absolute().parent.parent))

from apriltag.generate_nanopb import generate_nanopb
from apriltag.generate_quickbuf import generate_quickbuf
from shared.generation import GeneratorTypes, make_arg_parser


def main(argv):
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = make_arg_parser(
        dirname, dirname.parent, GeneratorTypes.NANOPB | GeneratorTypes.QUICKBUF
    )
    parser.set_defaults(proto_path=[dirname.parent / "wpimath/src/main/proto"])
    args = parser.parse_args(argv)

    generate_nanopb(
        args.nanopb,
        args.output_directory / "main/native/cpp",
        args.proto_directory,
        args.proto_path,
    )
    generate_quickbuf(
        args.protoc,
        args.quickbuf_plugin,
        args.output_directory / "main/java",
        args.proto_directory,
        args.proto_path,
    )


if __name__ == "__main__":
    main(sys.argv[1:])
