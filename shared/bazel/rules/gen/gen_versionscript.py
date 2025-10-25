import argparse

# Quick script to generate a version script for each OS from a list of symbols.


def main():
    parser = argparse.ArgumentParser(description="Generate a version script.")
    parser.add_argument("--input", required=True, help="Input symbols file")
    parser.add_argument("--output", required=True, help="Output version script file")
    parser.add_argument(
        "--lib_name", required=True, help="Name of the library for the version script"
    )
    parser.add_argument(
        "--format",
        required=True,
        choices=["linux", "windows", "osx"],
        help="Output format",
    )
    args = parser.parse_args()

    with open(args.input, "r") as f:
        symbols = f.read().splitlines()

    with open(args.output, "w") as f:
        if args.format == "linux":
            f.write(f"{args.lib_name} {{\n")
            f.write("  global: ")
            for symbol in symbols:
                symbol = symbol.strip()
                if symbol:
                    f.write(f"{symbol}; ")
            f.write("\n  local: *;\n")
            f.write("};\n")
        elif args.format == "windows":
            f.write(f"LIBRARY {args.lib_name}\n")
            f.write("EXPORTS\n")
            for symbol in symbols:
                symbol = symbol.strip()
                if symbol:
                    f.write(f"  {symbol}\n")
        elif args.format == "osx":
            for symbol in symbols:
                symbol = symbol.strip()
                if symbol:
                    f.write(f"_{symbol}\n")


if __name__ == "__main__":
    main()
