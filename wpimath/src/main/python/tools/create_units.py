#!/usr/bin/env python3

import io
import inspect
import pathlib
import re
import subprocess
import sys
import toml

out = {}

for f in sorted(pathlib.Path(sys.argv[1]).glob("*.h")):
    if f.name == "base.h":
        continue

    names = []

    with open(f) as fp:
        last_line = None
        for line in fp.readlines():
            line = line.strip()
            if last_line is not None:
                line = last_line + line
                last_line = None
            elif line.endswith("(") or line.endswith(","):
                last_line = line
                continue

            m = re.match(r"UNIT_ADD\(\w+, (\w+), (\w+),", line)
            if m:
                names.append((m.group(1), m.group(2)))
            else:
                m = re.match(r"UNIT_ADD_WITH_METRIC_PREFIXES\(\w+, (\w+), (\w+),", line)
                if m:
                    names.append((m.group(1), m.group(2)))
                    for i in ("nano", "micro", "milli", "kilo"):
                        names.append((f"{i}{m.group(1)}", f"{i}{m.group(2)}"))

    out_name = f"units_{f.name[:-2]}_type_caster.h"

    if names:
        if True:
            ofp = io.StringIO()

            ofp.write("#pragma once\n")
            ofp.write("\n")
            ofp.write(f"#include <units/{f.name}>\n")
            ofp.write("\n")

            ofp.write("\nnamespace pybind11 { namespace detail {\n")

            for single, double in names:
                ofp.write(
                    inspect.cleandoc(
                        f"""
                
                    template <> struct handle_type_name<units::{single}_t> {{
                    static constexpr auto name = _("{double}");
                    }};

                    template <> struct handle_type_name<units::{double}> {{
                    static constexpr auto name = _("{double}");
                    }};

                """
                    )
                )
                ofp.write("\n\n")

            ofp.write("\n}\n}\n\n")

            ofp.write(f'#include "_units_base_type_caster.h"\n')
            ofp.write("\n")

            ofp.seek(0)

            content = subprocess.check_output(
                ["clang-format"], input=ofp.getvalue(), encoding="utf-8"
            )

            with open(out_name, "w+") as fp:
                fp.write(content)
        else:
            out[out_name] = sorted(
                [f"units::{single}_t" for single, _ in names]
                + [f"units::{single}" for single, _ in names]
                + [f"units::{double}" for _, double in names]
            )


print(toml.dumps(dict(sup=out)))
