#!/usr/bin/env python3

import os
import os.path
import re
import sys
import time

from pathlib import Path


def process_file(content: str) -> str:
    # Update includes
    content = re.sub(
        r'#include "wpi/units/([^.]+)\.hpp"', r"#include <wpi/units/\1.h>", content
    )
    content = re.sub(
        r"#include <wpi/units/([^.]+)\.hpp>", r"#include <wpi/units/\1.h>", content
    )
    content = content.replace(
        "#include <wpi/units/base.h>", "#include <wpi/units/core.h>"
    )
    content = content.replace("#include <wpi/units/constants.h>\n", "")
    content = content.replace("#include <wpi/units/dimensionless.h>\n", "")
    content = content.replace("#include <wpi/units/math.h>\n", "")

    # Update math
    content = content.replace("units::math", "units")

    # Update unit
    content = re.sub("(?<=units::)unit(?=<)", "conversion_factor", content)
    content = re.sub("(?<=units::compound_)unit(?=<)", "conversion_factor", content)
    content = re.sub("(?<=units::traits::)base_unit_of(?=<)", "dimension_of_t", content)

    # Update to use concepts
    content = re.sub(
        "(?<=units::)(?:traits::)?is_unit_v", "ConversionFactorType", content
    )

    # Update unit_t
    content = content.replace("unit_t", "unit")

    # Update some UDLs
    content = content.replace("_mps_sq", "_mps2")
    content = content.replace("_fps_sq", "_fps2")

    # Remove explicit dimensionless namespace
    content = re.sub("(?<=units::)dimensionless::", "", content)

    # TODO Handle integer UDLs? Need to determine whether we want to modify upstream or usages

    # Update units
    UNITS: list[tuple[str, str]] = sorted(
        (
            (None, "dimensionless", None),
            (None, "scalar", "dimensionless"),
            ("meter", "meters", None),
            ("foot", "feet", None),
            ("second", "seconds", None),
            ("millisecond", "milliseconds", None),
            ("microsecond", "microseconds", None),
            ("radian", "radians", None),
            ("degree", "degrees", None),
            ("turn", "turns", None),
            ("kilogram", "kilograms", None),
            ("pound", "pounds", "mass::pounds"),
            ("ampere", "amperes", None),
            ("volt", "volts", None),
            ("ohm", "ohms", None),
            ("square_meter", "square_meters", None),
            ("newton_meter", "newton_meters", None),
            ("kilogram_square_meter", "kilogram_square_meters", None),
            (None, "meters_per_second", None),
            (None, "feet_per_second", None),
            (None, "meters_per_second_squared", None),
            (None, "feet_per_second_squared", None),
            (None, "radians_per_second", None),
            (None, "degrees_per_second", None),
            (None, "radians_per_second_squared", None),
            (None, "degrees_per_second_squared", None),
        ),
        key=lambda x: -len(x[1]),
    )
    for sing, plural, new_plural in UNITS:
        assert re.escape(plural) == plural
        if sing is None:
            sing = plural
        else:
            assert re.escape(sing) == sing
        if new_plural is None:
            new_plural = plural
        else:
            assert re.escape(new_plural) == new_plural
        # Update unit/conversion factor
        content = re.sub(f"(?<=units::){plural}(?![a-zA-Z0-9_])", f"{new_plural}_", content)
        content = re.sub(f"(?<=units::){sing}(?![a-zA-Z0-9_])", f"{new_plural}_", content)
        # Update unit_t/unit
        # TODO Use CTAD for variables (e.g., wpi::units::seconds x;)
        #   Note that the CTAD will misbehave when using ints
        old_content: str = content
        content = re.sub(f"(?<=units::){sing}_t", f"{new_plural}<>", content)

    return content


def files(*dirs: tuple[str]):
    for dirpath in dirs:
        if os.path.isfile(dirpath):
            yield Path(dirpath)
            continue
        for dp, dn, fn in os.walk(dirpath):
            dp = Path(dp)
            # Detect directory type
            valid_exts: tuple[str, ...] = ()
            kind: str = ""
            for part in reversed(dp.parts):
                if part in ("native", "cpp", "include"):
                    valid_exts = (".h", ".cpp", ".inc", ".hpp", ".c")
                    kind = "C++"
                    break
                elif part == "semiwrap":
                    valid_exts = (".yml",)
                    kind = "yml"
                    break
            # Process files in this directory
            fn.sort()
            if valid_exts:
                for f in fn:
                    if f.startswith("."):
                        print(f"Skipping {dp / f}")
                        continue
                    if not any(f.endswith(ext) for ext in valid_exts):
                        print(f"Skipping non-{kind} file {dp / f}")
                        continue
                    yield dp / f
            elif fn:
                files_str: str = "1 file" if len(fn) == 1 else f"{len(fn)} files"
                print(f"Skipping {files_str} in non-native directory {dp}")
            # Update the next directories to recurse into
            dn.sort()
            if dp.name == "python":
                i = 0
                while i < len(dn):
                    if dn[i] not in ("semiwrap", "cpp"):
                        print(f"Skipping {dp / dn[i]}")
                        del dn[i]
                        continue
                    i += 1
            else:
                for bad_dir in ("generate", "java", "resources", "thirdparty"):
                    if bad_dir in dn:
                        print(f"Skipping {dp / bad_dir}")
                        dn.remove(bad_dir)


def main():
    start: float = time.monotonic()
    changed_files_count: int = 0
    for path in files(*sys.argv[1:]):
        content: str
        with open(path) as f:
            content = f.read()

        old_content: str = str
        content = process_file(content)

        if content != old_content:
            changed_files_count += 1

        with open(path, "w") as f:
            f.write(content)
    end: float = time.monotonic()

    print(f"Done processing {changed_files_count} files in {end - start:.2f}s")


if __name__ == "__main__":
    main()
