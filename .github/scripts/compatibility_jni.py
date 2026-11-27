# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""JNI entry-point contracts and native-to-Java literal reference evidence."""

import re
import subprocess
import zipfile

from compatibility_binary import format_of, llvm_tool, symbols
from compatibility_java import jar_classes


def classes(snapshot):
    result = {}
    for jar in sorted((snapshot / "java").glob("*.jar")):
        for entry, cls in jar_classes(jar):
            if not entry.startswith("META-INF/versions/"):
                result[cls["name"]] = cls
    return result


def escape(name):
    """JNI escaping is defined over UTF-16 code units, including surrogate pairs."""
    result = []
    raw = name.encode("utf-16-be", "surrogatepass")
    special = {"/": "_", "_": "_1", ";": "_2", "[": "_3"}
    for index in range(0, len(raw), 2):
        code = int.from_bytes(raw[index : index + 2], "big")
        char = chr(code)
        result.append(
            char
            if char.isascii() and char.isalnum()
            else special.get(char, f"_0{code:04x}")
        )
    return "".join(result)


def bindings(java):
    result = {}
    for owner, cls in java.items():
        for (kind, name, descriptor), flags in cls["member_flags"].items():
            if kind == "method" and flags & 0x100:
                short = "Java_" + escape(owner) + "_" + escape(name)
                long = short + "__" + escape(descriptor[1 : descriptor.index(")")])
                result[(owner, name, descriptor)] = {
                    "short": short,
                    "long": long,
                    "static": bool(flags & 8),
                }
    return result


def java_changes(old, new):
    """JNI can access private members; include definitions beyond japicmp's API scope."""
    changes = []
    for owner, cls in old.items():
        if owner not in new:
            changes.append({"class": owner, "kind": "class removed"})
            continue
        current = new[owner]
        if bool(cls["flags"] & 0x200) != bool(current["flags"] & 0x200):
            changes.append({"class": owner, "kind": "class/interface changed"})
        for key, flags in cls["member_flags"].items():
            kind, name, descriptor = key
            if key not in current["member_flags"]:
                change = "member removed or descriptor changed"
            elif bool(flags & 8) != bool(current["member_flags"][key] & 8):
                change = "static/instance changed"
            else:
                continue
            changes.append(
                {
                    "class": owner,
                    "kind": change,
                    "member_kind": kind,
                    "name": name,
                    "descriptor": descriptor,
                }
            )
    return changes


def literals(data):
    # Co-occurrence is a review lead, not reconstruction of GetMethodID/RegisterNatives.
    return {
        match[:-1].decode("ascii")
        for match in re.findall(rb"[\x20-\x7e]{1,}\x00", data)
    }


def literal_findings(data, changes):
    if not changes:
        return []
    strings = literals(data)
    findings = []
    for change in changes:
        if change["class"] not in strings:
            continue
        if "name" in change and not {change["name"], change["descriptor"]} <= strings:
            continue
        findings.append(
            dict(
                change,
                evidence="native string literals; lookup association is not proven",
            )
        )
    return findings


def compare(base, head, old, new, vendor_artifacts, temporary, architecture=None):
    old_java, new_java = classes(base), classes(head)
    before, after = bindings(old_java), bindings(new_java)
    findings, errors = [], []
    old_exports, new_exports = set(), set()
    for library, path in old.items():
        exports = symbols(path, dynamic=True)[0]
        old_exports.update(exports)
        current = symbols(new[library], dynamic=True)[0] if library in new else set()
        for symbol in sorted(exports - current):
            if symbol.startswith("Java_") or symbol in ("JNI_OnLoad", "JNI_OnUnload"):
                findings.append(
                    {
                        "kind": "JNI export removed from library",
                        "library": library,
                        "symbol": symbol,
                    }
                )
    for path in new.values():
        new_exports.update(symbols(path, dynamic=True)[0])
    for key, binding in before.items():
        owner, name, desc = key
        if key not in after or binding["static"] != after[key]["static"]:
            findings.append(
                {
                    "kind": "Java native declaration changed",
                    "class": owner,
                    "name": name,
                    "descriptor": desc,
                }
            )
        elif {binding["short"], binding["long"]} & old_exports and not {
            binding["short"],
            binding["long"],
        } & new_exports:
            findings.append(
                {
                    "kind": "native binding no longer exported",
                    "class": owner,
                    "name": name,
                    "descriptor": desc,
                }
            )
    # Detect broken new bindings when this class already used exported JNI names.
    bound_classes = {
        key[0]
        for key, binding in before.items()
        if {binding["short"], binding["long"]} & old_exports
    }
    for key, binding in after.items():
        if (
            key not in before
            and key[0] in bound_classes
            and not {binding["short"], binding["long"]} & new_exports
        ):
            findings.append(
                {
                    "kind": "new native declaration has no matching export",
                    "class": key[0],
                    "name": key[1],
                    "descriptor": key[2],
                }
            )
    changes = java_changes(old_java, new_java)
    # Inspect both old WPILib binaries (compatibility) and new ones (stale JNI lookups).
    for side, libraries in (("baseline", old), ("candidate", new)):
        for library, path in libraries.items():
            refs = literal_findings(path.read_bytes(), changes)
            if refs:
                findings.append(
                    {
                        "kind": "WPILib native Java references",
                        "side": side,
                        "library": library,
                        "references": refs,
                    }
                )
    scanned = 0
    for number, (artifact, path) in enumerate(vendor_artifacts):
        try:
            with zipfile.ZipFile(path) as archive:
                found = False
                for entry in archive.infolist():
                    if not re.search(
                        r"\.(?:so(?:\.\d+)*|dylib|dll|a|lib)$", entry.filename
                    ):
                        continue
                    found = True
                    data = archive.read(entry)
                    if architecture:
                        target = temporary / f"jni-{number}"
                        target.write_bytes(data)
                        if format_of(target) == "macho":
                            arches = subprocess.check_output(
                                [llvm_tool("llvm-lipo"), "-archs", str(target)],
                                text=True,
                                timeout=120,
                            ).split()
                            if architecture not in arches:
                                raise ValueError(
                                    f"Missing {architecture} slice: {entry.filename}"
                                )
                            if len(arches) > 1:
                                thin = temporary / f"jni-{number}-thin"
                                subprocess.run(
                                    [
                                        llvm_tool("llvm-lipo"),
                                        str(target),
                                        "-thin",
                                        architecture,
                                        "-output",
                                        str(thin),
                                    ],
                                    check=True,
                                    capture_output=True,
                                    timeout=120,
                                )
                                data = thin.read_bytes()
                                thin.unlink()
                        target.unlink()
                    refs = literal_findings(data, changes)
                    if refs:
                        findings.append(
                            {
                                "kind": "vendor native Java references",
                                "artifact": artifact["path"],
                                "manifests": artifact["manifests"],
                                "entry": entry.filename,
                                "references": refs,
                            }
                        )
                if not found:
                    raise ValueError("No native binaries in vendor archive")
                scanned += 1
        except (
            OSError,
            ValueError,
            zipfile.BadZipFile,
            subprocess.SubprocessError,
        ) as error:
            errors.append(f"JNI vendor {artifact['path']}: {error}")
    return {
        "findings": findings,
        "errors": errors,
        "vendor_artifacts_scanned": scanned,
        "native_declarations": len(after),
        "exported_bindings": sum(
            bool({v["short"], v["long"]} & new_exports) for v in after.values()
        ),
        "coverage": "Static JNI names and literal references; computed lookups and dynamic registrations are not fully resolved.",
    }
