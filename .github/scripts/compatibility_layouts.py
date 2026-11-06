#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Snapshot Windows PDB/macOS DWARF types in a separate, bounded inspection process."""

import argparse
import json
import re
import subprocess
from pathlib import Path

from compatibility_binary import format_of, llvm_tool, symbols


def dwarf_virtual_methods(path):
    """Retain DWARF virtuality and slot expressions which SBType does not expose."""
    dump = subprocess.check_output(
        [llvm_tool("llvm-dwarfdump"), "--debug-info", str(path)], text=True, timeout=120
    )
    stack, methods = [], {}
    for block in re.split(r"(?=^0x[0-9A-Fa-f]+:)", dump, flags=re.MULTILINE):
        header = re.match(r"0x[0-9A-Fa-f]+:( +)(DW_TAG_\w+|NULL)", block)
        if not header:
            continue
        depth = len(header[1])
        while stack and stack[-1][0] >= depth:
            stack.pop()
        attrs = dict(re.findall(r"DW_AT_(\w+)\s+\((.*)\)", block))
        name = attrs.get("name", "").strip('"')
        owner = "::".join(value for _, value in stack if value)
        if "virtuality" in attrs:
            key = attrs.get("linkage_name", "").strip('"') or owner + "::" + name
            methods[key] = {
                "owner": owner,
                "virtuality": attrs["virtuality"],
                "slot": attrs.get("vtable_elem_location"),
            }
        is_scope = header[2] in (
            "DW_TAG_namespace",
            "DW_TAG_class_type",
            "DW_TAG_structure_type",
        )
        stack.append((depth, name if is_scope else ""))
    return methods


def pdb_types(debug, exports):
    """Resolve CodeView type indices to stable names/definitions, including virtual slots."""
    dump = subprocess.check_output(
        [llvm_tool("llvm-pdbutil"), "dump", "-types", str(debug)],
        text=True,
        timeout=120,
    )
    blocks, names, definitions = {}, {}, {}
    for block in re.split(r"(?=^[ \t]+0x[0-9A-Fa-f]+ \|)", dump, flags=re.MULTILINE):
        header = re.match(r"[ \t]+(0x[0-9A-Fa-f]+) \| (LF_\w+)", block)
        if not header:
            continue
        index = header[1]
        body = re.sub(r"unique name: `.*`", "", "\n".join(block.splitlines()[1:]))
        blocks[index] = header[2] + " " + re.sub(r"\s+", " ", body).strip()
        name = re.search(r"LF_(?:CLASS|STRUCTURE|UNION|ENUM) \[.*?\] `(.*)`", block)
        if name:
            names[index] = name[1]
            if "forward ref" not in block:
                definitions.setdefault(name[1], []).append(index)
    if not blocks:
        raise ValueError("PDB contains no type records")

    def resolve(index, seen=frozenset()):
        if index in names:
            return names[index]
        if int(index, 16) < 0x1000:
            return index  # CodeView primitive type IDs are stable across builds.
        if index in seen:
            return "<recursive type>"
        if index not in blocks:
            raise ValueError(f"Unresolved PDB type index: {index}")
        return expand(blocks[index], seen | {index})

    def expand(body, seen=frozenset()):
        return re.sub(
            r"`[^`]*`|\b(0x[0-9A-Fa-f]+)\b",
            lambda m: resolve(m[1], seen) if m[1] else m[0],
            body,
        )

    records = {}
    for name, indices in definitions.items():
        if not name.startswith(("std::", "__", "`", "<")):
            records[name] = sorted({expand(blocks[index]) for index in indices})
    # C-linkage exports do not encode their parameter/return types in their names.
    dump = subprocess.check_output(
        [llvm_tool("llvm-pdbutil"), "dump", "-symbols", "-globals", str(debug)],
        text=True,
        timeout=120,
    )
    functions = {}
    for match in re.finditer(
        r"S_[GL]PROC32 \[.*?\] `([^\n]+)`\n[^\n]*\n\s+type = `(0x[0-9A-Fa-f]+)", dump
    ):
        name, index = match.groups()
        if name in exports:
            functions.setdefault(name, set()).add(resolve(index))
    variables = {}
    for match in re.finditer(
        r"S_GDATA32 \[.*?\] `([^\n]+)`\n\s+type = (0x[0-9A-Fa-f]+)", dump
    ):
        name, index = match.groups()
        if name in exports:
            variables.setdefault(name, set()).add(resolve(index))
    return (
        records,
        {name: sorted(types) for name, types in functions.items()},
        {name: sorted(types) for name, types in variables.items()},
    )


def inspect(path):
    debug = (
        path.with_suffix(".pdb")
        if format_of(path) == "pe"
        else Path(str(path) + ".debug")
    )
    if not debug.is_file():
        raise ValueError(f"Missing debug file: {debug.name}")
    if format_of(path) == "pe":
        exports = symbols(path, dynamic=True)[0]
        records, functions, variables = pdb_types(debug, exports)
        return {
            "exports": sorted(exports),
            "records": records,
            "functions": functions,
            "variables": variables,
            "virtual_methods": {},
        }

    import lldb

    lldb.SBDebugger.Initialize()
    debugger = lldb.SBDebugger.Create(False)
    debugger.HandleCommand("settings set target.load-script-from-symbol-file false")
    target = debugger.CreateTarget("")
    spec = lldb.SBModuleSpec()
    spec.SetFileSpec(lldb.SBFileSpec(str(path)))
    spec.SetSymbolFileSpec(lldb.SBFileSpec(str(debug)))
    module = target.AddModule(spec)
    if not module.IsValid() or not module.GetNumCompileUnits():
        raise ValueError(f"No usable debug information in {path.name}")
    types = list(module.GetTypes())
    records = {}
    for typ in types:
        if not typ.IsTypeComplete() or not typ.GetName():
            continue
        if not (
            typ.IsAggregateType() or typ.GetTypeClass() == lldb.eTypeClassEnumeration
        ):
            continue
        if typ.GetName().startswith(("std::", "__")):
            continue

        def member(value):
            return {
                "name": value.GetName(),
                "type": value.GetType().GetCanonicalType().GetName(),
                "offset_bits": value.GetOffsetInBits(),
                "bitfield_bits": value.GetBitfieldSizeInBits(),
            }

        record = {
            "size": typ.GetByteSize(),
            "alignment": typ.GetByteAlign(),
            "fields": [
                member(typ.GetFieldAtIndex(i)) for i in range(typ.GetNumberOfFields())
            ],
            "bases": [
                member(typ.GetDirectBaseClassAtIndex(i))
                for i in range(typ.GetNumberOfDirectBaseClasses())
            ],
            "virtual_bases": [
                member(typ.GetVirtualBaseClassAtIndex(i))
                for i in range(typ.GetNumberOfVirtualBaseClasses())
            ],
            "polymorphic": typ.IsPolymorphicClass(),
            "methods": [
                typ.GetMemberFunctionAtIndex(i).GetType().GetName()
                + ":"
                + typ.GetMemberFunctionAtIndex(i).GetName()
                for i in range(typ.GetNumberOfMemberFunctions())
            ],
            "enum": [(v.GetName(), v.GetValueAsSigned()) for v in typ.GetEnumMembers()],
        }
        records.setdefault(typ.GetName(), [])
        if record not in records[typ.GetName()]:
            records[typ.GetName()].append(record)
    functions, variables = {}, {}
    exports = symbols(path, dynamic=True)[0]
    for name in sorted(exports):
        matches = module.FindFunctions(name)
        signatures = {
            matches.GetContextAtIndex(i).GetFunction().GetType().GetName()
            for i in range(matches.GetSize())
        }
        signatures.discard("")
        signatures.discard(None)
        if signatures:
            functions[name] = sorted(signatures)
        globals_ = module.FindGlobalVariables(target, name, 0xFFFFFFFF)
        variable_types = {
            v.GetType().GetCanonicalType().GetName()
            for v in globals_
            if v.GetType().IsValid()
        }
        if variable_types:
            variables[name] = sorted(variable_types)
    records = {
        name: sorted(variants, key=lambda v: json.dumps(v, sort_keys=True))
        for name, variants in records.items()
    }
    result = {
        "exports": sorted(exports),
        "records": records,
        "functions": functions,
        "variables": variables,
        "virtual_methods": (
            dwarf_virtual_methods(debug) if format_of(path) == "macho" else {}
        ),
    }
    lldb.SBDebugger.Destroy(debugger)
    return result


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("library", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    args.output.write_text(json.dumps(inspect(args.library), sort_keys=True) + "\n")
