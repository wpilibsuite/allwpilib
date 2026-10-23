# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Native symbol evidence for vendordep consumers of ABI findings."""

import re
import subprocess
import zipfile

from compatibility_binary import demangle, symbols


def diagnostic_context(text):
    """Evidence only: text/symbol matching is not a C++ type-equivalence proof."""
    types = set(re.findall(r"\b(?:class|struct|union) ([^'\n]+)'", text))
    # Redundant reporting retains all changed exported interfaces, including ones
    # whose underlying type change was explained under another interface.
    interfaces = re.findall(
        r"^  \[[DC]\] '(?:function|method|variable) ([^'\n]+)'", text, re.MULTILINE
    )

    def compact(value):
        return re.sub(r"\s+", "", value)

    types = {t for t in types if not t.startswith(("std::", "__"))}
    signatures = []
    for interface in interfaces:
        boundaries = set()
        position = 0
        for index, char in enumerate(interface):
            if char.isspace():
                continue
            if index == 0 or not re.match(r"[\w:]", interface[index - 1]):
                boundaries.add(position)
            position += 1
        signatures.append((compact(interface), boundaries))
    return {compact(t): t for t in types}, signatures


def trace_native(artifacts, old, new, results, output, temporary, architecture=None):
    """Join removed exports and diagnostic names with shared/static vendor symbols."""
    exports, current = {}, set()
    for library, path in old.items():
        for symbol in symbols(path, dynamic=True)[0]:
            exports.setdefault(symbol, []).append(library)
    for path in new.values():
        current.update(symbols(path, dynamic=True)[0])
    removed = exports.keys() - current
    contexts = []
    for result in results:
        if result["status"] == "changed":
            text = (output / result["report"]).read_text()
            raw_interfaces = set(
                re.findall(
                    r"^(?:Function signature changed|Virtual method ABI changed|Variable type changed): ([^:\n]+)",
                    text,
                    re.MULTILINE,
                )
            )
            contexts.append((result, diagnostic_context(text), raw_interfaces))
    findings, errors, warnings, undemangled = [], [], [], []
    scanned = 0
    for number, (artifact, path) in enumerate(artifacts):
        try:
            found = False
            with zipfile.ZipFile(path) as archive:
                for entry_number, entry in enumerate(archive.infolist()):
                    if not re.search(
                        r"\.(?:a|lib|dll|dylib|so(?:\.\d+)*)$", entry.filename
                    ):
                        continue
                    found = True
                    # Use generated filenames, never the archive's paths.
                    target = temporary / f"vendor-{number}-{entry_number}"
                    target.write_bytes(archive.read(entry))
                    is_shared = (
                        entry.filename.endswith((".dll", ".dylib"))
                        or ".so" in entry.filename
                    )
                    defined, undefined = symbols(
                        target, dynamic=is_shared, architecture=architecture
                    )
                    if not defined and not undefined:
                        raise ValueError(f"No symbols in {entry.filename}")
                    evidence = []
                    for symbol in sorted(undefined & removed):
                        evidence.append(
                            {
                                "kind": "removed symbol",
                                "symbol": symbol,
                                "libraries": exports[symbol],
                            }
                        )
                    names, undecoded = demangle(defined | undefined)
                    if undecoded:
                        warnings.append(
                            f"Native vendor {artifact['path']} ({entry.filename}): "
                            f"{len(undecoded)} symbols could not be demangled; "
                            "raw symbol matching is retained."
                        )
                        undemangled.append(
                            {
                                "artifact": artifact["path"],
                                "entry": entry.filename,
                                "symbols": undecoded,
                            }
                        )
                    for result, (types, interfaces), raw_interfaces in contexts:
                        matches = []
                        pattern = (
                            re.compile(
                                r"(?<![\w:])(?:"
                                + "|".join(
                                    re.escape(t)
                                    for t in sorted(types, key=len, reverse=True)
                                )
                                + r")(?![\w])"
                            )
                            if types
                            else None
                        )
                        for raw, name in names.items():
                            compact = re.sub(r"\s+", "", name)
                            if raw in undefined and (
                                raw in raw_interfaces
                                or any(
                                    i.endswith(compact)
                                    and len(i) - len(compact) in boundaries
                                    for i, boundaries in interfaces
                                )
                            ):
                                matches.append(
                                    {
                                        "kind": "changed interface reference",
                                        "symbol": raw,
                                        "demangled": name,
                                    }
                                )
                            # Defined inline/template symbols can embed old layouts.
                            for typename in sorted(
                                {types[m.group()] for m in pattern.finditer(compact)}
                                if pattern
                                else set()
                            ):
                                matches.append(
                                    {
                                        "kind": "type mentioned in ABI diagnostic",
                                        "type": typename,
                                        "symbol": raw,
                                        "demangled": name,
                                    }
                                )
                        if matches:
                            evidence.append(
                                {
                                    "kind": "ABI diagnostic references",
                                    "library": result["library"],
                                    "report": result["report"],
                                    "matches": matches,
                                }
                            )
                    if evidence:
                        findings.append(
                            {
                                "artifact": artifact["path"],
                                "manifests": artifact["manifests"],
                                "entry": entry.filename,
                                "references": evidence,
                            }
                        )
                    target.unlink()
            if not found:
                raise ValueError("Archive contains no shared or static libraries")
            scanned += 1
        except (
            OSError,
            ValueError,
            zipfile.BadZipFile,
            subprocess.SubprocessError,
        ) as error:
            errors.append(f"Native vendor {artifact['path']}: {error}")
    return {
        "scanned": scanned,
        "findings": findings,
        "errors": errors,
        "warnings": warnings,
        "undemangled": undemangled,
    }
