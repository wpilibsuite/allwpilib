#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Compare compatibilitySnapshot outputs; findings are advisory, tool errors are not."""

import argparse
import hashlib
import html
import json
import os
from pathlib import Path
import subprocess
import tempfile
import xml.etree.ElementTree as ET
import zipfile


def previous_tag(repo, head):
    """Find the nearest release tag on the branch's history, strictly before head."""
    return subprocess.check_output(
        [
            "git",
            "-C",
            str(repo),
            "describe",
            "--tags",
            "--first-parent",
            "--match",
            "v[0-9]*",
            "--abbrev=0",
            f"{head}^",
        ],
        text=True,
    ).strip()


def run_tool(command, log):
    """Keep full diagnostics, including failures and timeouts, in the artifact."""
    with log.open("w", encoding="utf-8") as stream:
        try:
            return subprocess.run(
                command,
                stdout=stream,
                stderr=subprocess.STDOUT,
                timeout=1200,
                check=False,
            ).returncode
        except (OSError, subprocess.TimeoutExpired) as error:
            stream.write(f"\n{error}\n")
            return -1


def native_status(returncode):
    """libabigail uses a bitmask; ABI changes (4/8) are distinct from errors (1/2)."""
    if returncode < 0 or returncode & ~12:
        return "error"
    return "changed" if returncode else "unchanged"


def native_libraries(snapshot, destination):
    """Extract only ELF libraries and adjacent debug files, without trusting zip paths."""
    libraries = {}
    archives = sorted((snapshot / "native").glob("*.zip"))
    if not archives:
        raise ValueError(f"No native archives in {snapshot}")
    for archive in archives:
        found = False
        with zipfile.ZipFile(archive) as contents:
            for entry in contents.infolist():
                name = Path(entry.filename).name
                if not (name.endswith(".so") or name.endswith(".so.debug")):
                    continue
                target = destination / archive.stem / name
                target.parent.mkdir(parents=True, exist_ok=True)
                if target.exists():
                    raise ValueError(f"Duplicate native file in {archive}: {name}")
                target.write_bytes(contents.read(entry))
                if name.endswith(".so"):
                    with target.open("rb") as stream:
                        if stream.read(4) != b"\x7fELF":
                            raise ValueError(f"Not an ELF library: {target}")
                    libraries[f"{archive.stem}/{name}"] = target
                    found = True
        if not found:
            raise ValueError(f"No shared libraries in {archive}")
    return libraries


def has_debug_info(library):
    """Check explicitly: abidiff can short-circuit identical, stripped inputs."""
    for path in (library, Path(str(library) + ".debug")):
        if path.is_file():
            sections = subprocess.check_output(
                ["readelf", "--sections", "--wide", str(path)],
                text=True,
                stderr=subprocess.STDOUT,
                timeout=30,
            )
            if ".debug_info " in sections or ".zdebug_info " in sections:
                return True
    return False


def compare_native(base, head, output, abidiff):
    with tempfile.TemporaryDirectory(prefix="native-", dir=output) as directory:
        old = native_libraries(base, Path(directory) / "base")
        new = native_libraries(head, Path(directory) / "head")
        return compare_native_libraries(old, new, output, abidiff)


def identical_native_files(old, new):
    """Avoid expensive DWARF analysis when both the binary and debug file are identical."""
    for suffix in ("", ".debug"):
        left, right = Path(str(old) + suffix), Path(str(new) + suffix)
        if not left.exists() and not right.exists():
            continue
        if not left.exists() or not right.exists():
            return False
        if left.stat().st_size != right.stat().st_size:
            return False
        with left.open("rb") as a, right.open("rb") as b:
            if (
                hashlib.file_digest(a, "sha256").digest()
                != hashlib.file_digest(b, "sha256").digest()
            ):
                return False
    return True


def compare_native_libraries(old, new, output, abidiff):
    results = []
    for name in sorted(old.keys() | new.keys()):
        log = output / (name.replace("/", "-") + ".txt")
        if name not in new:
            status = "removed"
            log.write_text(f"Published library removed: {name}\n", encoding="utf-8")
        elif name not in old:
            status = "added"
            log.write_text(f"Published library added: {name}\n", encoding="utf-8")
        elif not has_debug_info(old[name]) or not has_debug_info(new[name]):
            status = "error"
            log.write_text(
                f"Missing DWARF debug information: {name}\n", encoding="utf-8"
            )
        elif identical_native_files(old[name], new[name]):
            status = "unchanged"
            log.write_text("Identical library and debug information (SHA-256).\n")
        else:
            code = run_tool(
                [
                    abidiff,
                    "--fail-no-debug-info",
                    "--no-default-suppression",
                    "--exported-interfaces-only",
                    "--no-added-syms",
                    "--no-show-locs",
                    "--show-bytes",
                    str(old[name]),
                    str(new[name]),
                ],
                log,
            )
            status = native_status(code)
        result = {"library": name, "status": status, "report": log.name}
        if status in ("changed", "removed", "error"):
            with log.open(encoding="utf-8", errors="replace") as stream:
                result["excerpt"] = "\n".join(stream.read(6000).splitlines()[:40])
        results.append(result)
    return results


def java_classpath(snapshot):
    return os.pathsep.join(str(p) for p in sorted(snapshot.rglob("*.jar")))


def java_findings(xml):
    """Keep source-only changes too, e.g. a changed generic return element type."""
    findings = []
    for cls in ET.parse(xml).getroot().findall("./classes/class"):
        binary = cls.get("binaryCompatible") == "false"
        source = cls.get("sourceCompatible") == "false"
        if binary or source:
            findings.append(
                {
                    "class": cls.attrib["fullyQualifiedName"],
                    "binary": binary,
                    "source": source,
                    "changes": sorted(
                        {
                            change.attrib["type"]
                            for change in cls.findall(".//compatibilityChange")
                            if change.get("binaryCompatible") == "false"
                            or change.get("sourceCompatible") == "false"
                        }
                    ),
                }
            )
    return findings


def compare_java(base, head, output, japicmp):
    old = sorted((base / "java").glob("*.jar"))
    new = sorted((head / "java").glob("*.jar"))
    if not old or not new:
        raise ValueError("Both snapshots must contain published Java jars")
    xml = output / "java.xml"
    code = run_tool(
        [
            "java",
            "-jar",
            str(japicmp),
            "--old",
            ";".join(map(str, old)),
            "--new",
            ";".join(map(str, new)),
            "--old-classpath",
            java_classpath(base),
            "--new-classpath",
            java_classpath(head),
            "-a",
            "protected",
            "--only-modified",
            "--report-only-filename",
            "--xml-file",
            str(xml),
            "--html-file",
            str(output / "java.html"),
        ],
        output / "java.txt",
    )
    # No --error-on-* switches: nonzero here is a tool/classpath error, not a finding.
    if code:
        raise ValueError(f"japicmp failed ({code}); see java.txt")
    return java_findings(xml)


def summary(report):
    lines = [
        "# Compatibility report (advisory)",
        "",
        f"Baseline: `{html.escape(report['base'])}`  ",
        f"Candidate: `{html.escape(report['head'])}`",
        "",
        "Scope: published Java public/protected definitions and Linux x86-64 "
        "C++ shared-library ABI, including object layouts reachable from exported interfaces.",
        "These are potential client/vendor breakages; vendor usage is not verified. "
        "A clean report does not establish compatibility on other platforms or for "
        "header-only C++ code, inline behavior, reflection, or other runtime semantics.",
        "",
    ]
    if report["errors"]:
        lines += ["**INCOMPLETE: some checks could not run.**", ""]
        lines += [f"- {html.escape(error)}" for error in report["errors"]]
        lines.append("")
    native = report["native"]
    changed = [r for r in native if r["status"] in ("changed", "removed")]
    lines += [
        f"## C++: {len(changed)} libraries with ABI findings ({len(native)} inventoried)",
        "",
    ]
    for result in native:
        if result["status"] not in ("unchanged", "added"):
            lines.append(
                f"- `{result['library']}`: **{result['status']}** "
                f"(artifact: `{result['report']}`)"
            )
            if result.get("excerpt"):
                lines += [
                    "",
                    "<details><summary>Diagnostic excerpt (full report in artifact)</summary>",
                    "",
                    f"<pre>{html.escape(result['excerpt'])}</pre>",
                    "</details>",
                    "",
                ]
    java = report["java"]
    lines += ["", f"## Java: {len(java)} classes with compatibility findings", ""]
    for result in java[:100]:
        kind = "binary" if result["binary"] else "source only"
        changes = ", ".join(result["changes"])
        lines.append(f"- `{result['class']}` ({kind}): {changes}")
    if len(java) > 100:
        lines.append("- Further findings are in the full artifact.")
    lines += [
        "",
        "Download the compatibility artifact for full C++ diagnostics, Java HTML/XML, "
        "and machine-readable results.json. Findings do not block merging.",
        "",
    ]
    return "\n".join(lines)


def compare(args):
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    report = {
        "base": args.base_revision,
        "head": args.head_revision,
        "native": [],
        "java": [],
        "errors": [],
    }
    try:
        for snapshot in (args.base, args.head):
            if not (snapshot / "complete").is_file():
                raise ValueError(f"Snapshot did not finish: {snapshot}")
        try:
            report["native"] = compare_native(
                args.base, args.head, output, args.abidiff
            )
            for result in report["native"]:
                if result["status"] == "error":
                    report["errors"].append(f"abidiff failed: {result['library']}")
        except (
            OSError,
            ValueError,
            zipfile.BadZipFile,
            subprocess.SubprocessError,
        ) as error:
            report["errors"].append(f"C++: {error}")
        try:
            report["java"] = compare_java(args.base, args.head, output, args.japicmp)
        except (OSError, ValueError, ET.ParseError) as error:
            report["errors"].append(f"Java: {error}")
    except ValueError as error:
        report["errors"].append(str(error))
    (output / "results.json").write_text(json.dumps(report, indent=2) + "\n")
    (output / "summary.md").write_text(summary(report), encoding="utf-8")
    return int(bool(report["errors"]))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    tag = commands.add_parser("previous-tag")
    tag.add_argument("--repo", type=Path, default=Path("."))
    tag.add_argument("--head", default="HEAD")
    diff = commands.add_parser("compare")
    diff.add_argument("--base", type=Path, required=True)
    diff.add_argument("--head", type=Path, required=True)
    diff.add_argument("--base-revision", required=True)
    diff.add_argument("--head-revision", required=True)
    diff.add_argument("--output", type=Path, required=True)
    diff.add_argument("--japicmp", type=Path, required=True)
    diff.add_argument("--abidiff", default="abidiff")
    args = parser.parse_args()
    if args.command == "previous-tag":
        print(previous_tag(args.repo, args.head))
        return 0
    return compare(args)


if __name__ == "__main__":
    raise SystemExit(main())
