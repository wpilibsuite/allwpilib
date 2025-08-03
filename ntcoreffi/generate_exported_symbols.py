#!/usr/bin/python
import argparse
from pathlib import Path


def generate_symbol_text(output_directory, symbols):
    with open(output_directory, "w") as out:
        with open(symbols, "r") as f:
            for line in f:
                out.write(f"AddFunctionToLink({line.strip()});\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--symbols",
        help="Read the symbols from this file",
        type=Path,
    )
    parser.add_argument(
        "--output",
        help="Output the generated symbols to this file",
        type=Path,
    )
    args = parser.parse_args()

    generate_symbol_text(args.output, args.symbols)


if __name__ == "__main__":
    main()
