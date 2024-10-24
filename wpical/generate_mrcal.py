#!/usr/bin/env python3
import argparse
import subprocess
import sys
from pathlib import Path


def main(argv):
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        help="Optional. If set, will output the generated files to this directory, otherwise it will use a path relative to the script",
        default=dirname / "src/main/native/thirdparty/mrcal/generated/",
        type=Path,
    )
    args = parser.parse_args(argv)

    args.output_directory.mkdir(parents=True, exist_ok=True)
    result = subprocess.run(
        f"{dirname}/src/main/native/thirdparty/mrcal/src/minimath/minimath_generate.pl",
        capture_output=True,
    )
    (args.output_directory / "minimath_generated.h").write_text(
        str(result.stdout, encoding="UTF8")
    )


if __name__ == "__main__":
    main(sys.argv[1:])
