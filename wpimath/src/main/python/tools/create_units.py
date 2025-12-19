#!/usr/bin/env python3

import io
import inspect
import pathlib
import re
import subprocess
import sys


for f in sorted(pathlib.Path(sys.argv[1]).glob("*.h")):
    if f.name == "core.h":
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

            m = re.match(r"UNIT_ADD(?:_WITH_PLURAL_CONSTANT)?\(\w+, (\w+),", line)
            if m:
                names.append(m.group(1))
            else:
                m = re.match(r"UNIT_ADD_WITH_METRIC_PREFIXES\(\w+, (\w+),", line)
                if m:
                    names.append(m.group(1))
                    prefixes = ("nano", "micro", "milli", "kilo")
                    if m.group(1) == "meters":
                        prefixes = ("nano", "micro", "milli", "centi", "kilo")
                    for i in prefixes:
                        names.append(f"{i}{m.group(1)}")

    out_name = f"units_{f.stem}_type_caster.h"

    if names:
        if True:
            ofp = io.StringIO()

            ofp.write("#pragma once\n")
            ofp.write("\n")
            ofp.write(f"#include <wpi/units/{f.name}>\n")
            ofp.write("\n")

            ofp.write("namespace pybind11 {\n")
            ofp.write("namespace detail {\n")

            # "wpimath.units." renames:
            #   luxes -> lux
            #   nauticalMiles -> nautical_miles
            #   astronicalUnits -> astronomical_units
            #   nauticalLeagues -> nautical_leagues
            #   metric_tons -> tonnes
            #   mbars -> millibars
            #   rads -> radiation_absorbed_dose
            #   moles -> mols
            # We also need to disambiguiate the different pounds units:
            #   pounds (mass) -> pounds_mass
            #   pounds (force) -> pounds_force
            for name in names:
                code_name: str = name
                py_name: str = name
                if name == "pounds":
                    code_name = f.stem + "::" + name
                    py_name = name + "_" + f.stem
                s = (
                    inspect.cleandoc(
                        f"""

                    template <> struct handle_type_name<wpi::units::{code_name}<>> {{
                      static constexpr auto name = _("wpimath.units.{py_name}");
                    }};

                    template <> struct handle_type_name<wpi::units::{code_name}_> {{
                      static constexpr auto name = _("wpimath.units.{py_name}");
                    }};

                """
                    )
                    + "\n"
                )
                if name == "foot_pounds" and f.name == "torque.h":
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
            for name in sorted(names):
                if name == "pounds":
                    name = f.stem + "::" + name
                print(f'    "wpi::units::{name}",')
            print("]")
            print("default_arg_cast = true")
            print()
            print("[[tool.semiwrap.export_type_casters.wpimath-casters.headers]]")
            print(f'header = "{out_name}"')
            print("types = [")
            for name in sorted(names):
                if name == "pounds":
                    name = f.stem + "::" + name
                print(f'    "wpi::units::{name}_",')
            print("]")
            print("default_arg_cast = false")
            print()
