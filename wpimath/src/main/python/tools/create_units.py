#!/usr/bin/env python3

import io
import inspect
import pathlib
import re
import subprocess
import sys


for f in sorted(pathlib.Path(sys.argv[1]).glob("*.hpp")):
    if f.name == "base.hpp" or f.name == "angular_jerk.hpp":
        continue

    names = []

    with open(f) as fp:
        last_line = None
        for line in fp:
            line = line.strip()
            # Handle args on a different line from the opening parenthesis
            if last_line is not None:
                line = last_line + line
                last_line = None
            elif line.endswith("("):
                last_line = line
                continue
            elif line.endswith(","):
                last_line = line + " "
                continue

            m = re.match(r"UNIT_ADD\(\w+, (\w+), (\w+),", line)
            if m:
                if (
                    m.group(1) != "revolutions_per_minute_squared"
                    and m.group(1) != "revolutions_per_minute_per_second"
                ):
                    names.append((m.group(1), m.group(2)))
            else:
                m = re.match(r"UNIT_ADD_WITH_METRIC_PREFIXES\(\w+, (\w+), (\w+),", line)
                if m:
                    names.append((m.group(1), m.group(2)))
                    prefixes = ("nano", "micro", "milli", "kilo")
                    if m.group(1) == "meter":
                        prefixes = ("nano", "micro", "milli", "centi", "kilo")
                    for i in prefixes:
                        names.append((f"{i}{m.group(1)}", f"{i}{m.group(2)}"))

    out_name = f"units_{f.stem}_type_caster.h"

    if names:
        if True:
            ofp = io.StringIO()

            ofp.write("#pragma once\n")
            ofp.write("\n")
            ofp.write(f'#include "wpi/units/{f.name}"\n')
            ofp.write("\n")

            ofp.write("namespace pybind11 {\n")
            ofp.write("namespace detail {\n")

            for single, double in names:
                s = (
                    inspect.cleandoc(
                        f"""

                    template <> struct handle_type_name<wpi::units::{single}_t> {{
                      static constexpr auto name = _("wpimath.units.{double}");
                    }};

                    template <> struct handle_type_name<wpi::units::{double}> {{
                      static constexpr auto name = _("wpimath.units.{double}");
                    }};

                """
                    )
                    + "\n"
                )
                if single == "foot_pound" and f.name == "torque.hpp":
                    # Comment out non-blank lines
                    s = re.sub(r"(?:(?<=\n)|^)([^\n])", r"// \1", s)
                ofp.write(s)
                ofp.write("\n")

            ofp.write("} // namespace detail\n")
            ofp.write("} // namespace pybind11\n")
            ofp.write("\n")

            ofp.write(f'#include "_units_base_type_caster.h"\n')

            ofp.seek(0)

            content = ofp.getvalue()

            with open(pathlib.Path(sys.argv[2]) / out_name, "w+") as fp:
                fp.write(content)

            print("[[tool.semiwrap.export_type_casters.wpimath-casters.headers]]")
            print(f'header = "{out_name}"')
            print("types = [")
            for name in sorted(single for single, _ in names):
                print(f'    "wpi::units::{name}_t",')
            print("]")
            print("default_arg_cast = true")
            print()
            print("[[tool.semiwrap.export_type_casters.wpimath-casters.headers]]")
            print(f'header = "{out_name}"')
            print("types = [")
            for name in sorted(
                name for single, double in names for name in (single, double)
            ):
                print(f'    "wpi::units::{name}",')
            print("]")
            print("default_arg_cast = false")
            print()
