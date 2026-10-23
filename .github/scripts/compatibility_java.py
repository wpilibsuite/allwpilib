# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Static Java constant-pool references, matched to japicmp's incompatible definitions."""

import json
import re
import struct
import xml.etree.ElementTree as ET
import zipfile


def parse_class(data):
    """Read symbolic references and hierarchy without loading or executing vendor code."""
    position = 0

    def take(count):
        nonlocal position
        if position + count > len(data):
            raise ValueError("Truncated Java class")
        result = data[position : position + count]
        position += count
        return result

    def integer(count):
        return int.from_bytes(take(count), "big")

    if take(4) != b"\xca\xfe\xba\xbe":
        raise ValueError("Invalid Java class magic")
    take(4)
    pool = [None] * integer(2)
    index = 1
    while index < len(pool):
        tag = integer(1)
        if tag == 1:
            # Class files use modified UTF-8 (NUL and surrogate pairs differ from UTF-8).
            raw = take(integer(2)).replace(b"\xc0\x80", b"\x00")
            value = (
                raw.decode("utf-8", "surrogatepass")
                .encode("utf-16", "surrogatepass")
                .decode("utf-16", "surrogatepass")
            )
            pool[index] = (tag, value)
        elif tag in (3, 4):
            take(4)
        elif tag in (5, 6):
            take(8)
            index += 1
        elif tag in (7, 8, 16, 19, 20):
            pool[index] = (tag, integer(2))
        elif tag in (9, 10, 11, 12, 17, 18):
            pool[index] = (tag, integer(2), integer(2))
        elif tag == 15:
            pool[index] = (tag, integer(1), integer(2))
        else:
            raise ValueError(f"Unknown Java constant-pool tag {tag}")
        index += 1

    def utf(index):
        return pool[index][1]

    def cls(index):
        return utf(pool[index][1]) if index else None

    class_flags = integer(2)  # access flags
    name = cls(integer(2))
    parents = [cls(integer(2))]
    parents += [cls(integer(2)) for _ in range(integer(2))]
    references = set()
    types = set()
    for entry in pool:
        if not entry:
            continue
        if entry[0] in (9, 10, 11):
            member = pool[entry[2]]
            descriptor = utf(member[2])
            references.add(
                (
                    "field" if entry[0] == 9 else "method",
                    cls(entry[1]),
                    utf(member[1]),
                    descriptor,
                )
            )
            types.update(re.findall(r"L([^;]+);", descriptor))
        elif entry[0] == 7:
            value = utf(entry[1])
            types.update(
                re.findall(r"L([^;]+);", value) if value.startswith("[") else [value]
            )
        elif entry[0] == 16:
            types.update(re.findall(r"L([^;]+);", utf(entry[1])))
    members = set()
    member_flags = {}
    for kind in ("field", "method"):
        for _ in range(integer(2)):
            flags = integer(2)
            member, descriptor = utf(integer(2)), utf(integer(2))
            members.add((kind, member, descriptor))
            member_flags[(kind, member, descriptor)] = flags
            types.update(re.findall(r"L([^;]+);", descriptor))
            for _ in range(integer(2)):
                integer(2)
                take(integer(4))
    return {
        "name": name,
        "parents": [p for p in parents if p],
        "refs": references,
        "types": types,
        "members": members,
        "member_flags": member_flags,
        "flags": class_flags,
    }


def jar_classes(path):
    """Keep every multi-release variant as separate evidence, conservatively."""
    with zipfile.ZipFile(path) as archive:
        for entry in archive.infolist():
            if entry.filename.endswith(".class"):
                try:
                    yield entry.filename, parse_class(archive.read(entry))
                except (IndexError, TypeError, UnicodeError, struct.error) as error:
                    raise ValueError(
                        f"Invalid class {entry.filename} in {path}: {error}"
                    ) from error


def descriptor(java_type):
    primitives = {
        "void": "V",
        "boolean": "Z",
        "byte": "B",
        "char": "C",
        "short": "S",
        "int": "I",
        "long": "J",
        "float": "F",
        "double": "D",
    }
    if java_type.endswith("[]"):
        return "[" + descriptor(java_type[:-2])
    return primitives.get(java_type, "L" + java_type.replace(".", "/") + ";")


def incompatible(node):
    return (
        node.get("binaryCompatible") == "false"
        or node.get("sourceCompatible") == "false"
    )


def changed_definitions(xml):
    """Separate class-level changes from exact changed member signatures."""
    classes, members = {}, {}
    for cls in ET.parse(xml).getroot().findall("./classes/class"):
        owner = cls.attrib["fullyQualifiedName"].replace(".", "/")
        for child in cls:
            if child.tag in ("methods", "constructors", "fields"):
                continue
            changes = {
                c.attrib["type"]
                for c in child.iter("compatibilityChange")
                if incompatible(c)
            }
            if changes:
                classes.setdefault(owner, set()).update(changes)
        for node in (
            cls.findall("./methods/method")
            + cls.findall("./constructors/constructor")
            + cls.findall("./fields/field")
        ):
            changes = {
                c.attrib["type"]
                for c in node.iter("compatibilityChange")
                if incompatible(c)
            }
            if not changes:
                continue
            if node.get("changeStatus") == "NEW":
                classes.setdefault(owner, set()).update(changes)
                continue
            name = node.get("name")
            kind = "field" if node.tag == "field" else "method"
            if kind == "field":
                value = node.find("type").get("oldValue")
                desc = descriptor(value)
            else:
                desc = (
                    "("
                    + "".join(
                        descriptor(p.attrib["type"])
                        for p in node.findall("./parameters/parameter")
                    )
                    + ")"
                )
                if node.tag == "constructor":
                    name = "<init>"
                    desc += "V"
                else:
                    desc += descriptor(node.find("returnType").attrib["oldValue"])
            members[(kind, owner, name, desc)] = changes
    return classes, members


def class_references(cls, hierarchy, class_changes, member_changes):
    def ancestors(owner, seen=None):
        seen = set() if seen is None else seen
        if owner in seen:
            return
        seen.add(owner)
        yield owner
        for parent in hierarchy.get(owner, {}).get("parents", []):
            yield from ancestors(parent, seen)

    evidence = []
    # Include ancestors: an added abstract method can affect an existing subclass.
    types = set(cls["types"])
    for parent in cls["parents"]:
        types.update(ancestors(parent))
    for owner in sorted(types & class_changes.keys()):
        evidence.append(
            {
                "kind": "class",
                "target": owner.replace("/", "."),
                "changes": sorted(class_changes[owner]),
            }
        )
    for kind, owner, name, desc in sorted(cls["refs"]):
        for declaration in ancestors(owner):
            key = (kind, declaration, name, desc)
            if key in member_changes:
                evidence.append(
                    {
                        "kind": kind,
                        "target": f"{declaration.replace('/', '.')}.{name}{desc}",
                        "reference_owner": owner.replace("/", "."),
                        "changes": sorted(member_changes[key]),
                    }
                )
                break
            # Overridden methods must not be attributed to a changed superclass.
            if (kind, name, desc) in hierarchy.get(declaration, {}).get(
                "members", set()
            ) or name == "<init>":
                break
    return evidence


def trace_java(artifacts, base, xml):
    """Match references through each manifest's hierarchy, without mixing versions."""
    class_changes, member_changes = changed_definitions(xml)
    baseline = {}
    for path in sorted(base.rglob("*.jar")):
        for _, cls in jar_classes(path):
            baseline.setdefault(cls["name"], cls)
    parsed, errors = [], []
    hierarchies = {}
    for artifact, path in artifacts:
        try:
            classes = list(jar_classes(path))
            parsed.append((artifact, classes))
            for manifest in artifact["manifests"]:
                hierarchy = hierarchies.setdefault(manifest, dict(baseline))
                hierarchy.update((cls["name"], cls) for _, cls in classes)
        except (OSError, ValueError, zipfile.BadZipFile) as error:
            errors.append(f"Java vendor {artifact['path']}: {error}")
    findings = {}
    for artifact, classes in parsed:
        for manifest in artifact["manifests"]:
            for entry, cls in classes:
                evidence = class_references(
                    cls, hierarchies[manifest], class_changes, member_changes
                )
                if not evidence:
                    continue
                key = (artifact["path"], entry, json.dumps(evidence, sort_keys=True))
                finding = findings.setdefault(
                    key,
                    {
                        "artifact": artifact["path"],
                        "manifests": [],
                        "class": cls["name"].replace("/", "."),
                        "entry": entry,
                        "references": evidence,
                    },
                )
                finding["manifests"].append(manifest)
    return {
        "scanned": len(parsed),
        "findings": list(findings.values()),
        "errors": errors,
    }
