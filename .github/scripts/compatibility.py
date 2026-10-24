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
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET
import zipfile
from pathlib import Path

import compatibility_java
import compatibility_jni
import compatibility_native
import compatibility_vendors
from compatibility_binary import format_of, llvm_tool
from compatibility_process import run_tool


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


def native_status(returncode):
    """libabigail uses a bitmask; ABI changes (4/8) are distinct from errors (1/2)."""
    if returncode < 0 or returncode & ~12:
        return "error"
    return "changed" if returncode else "unchanged"


def native_files(snapshot, archive):
    """Read packaged libraries and debug sidecars added to the normal CI artifact."""
    suffixes = (".so", ".so.debug", ".dll", ".pdb", ".dylib", ".dylib.debug")
    packaged = set()
    with zipfile.ZipFile(archive) as contents:
        for entry in contents.infolist():
            if entry.filename.endswith(suffixes):
                packaged.add(Path(entry.filename).name)
                yield entry.filename, contents.read(entry)
    for sidecar in sorted((snapshot / "native-debug" / archive.stem).glob("*.debug")):
        if sidecar.name not in packaged and sidecar.name.endswith(
            (".so.debug", ".dylib.debug")
        ):
            yield sidecar.name, sidecar.read_bytes()


def native_libraries(snapshot, destination, architecture=None):
    """Extract shared libraries and their debug files without trusting zip paths."""
    libraries = {}
    archives = sorted((snapshot / "native").glob("*.zip"))
    if not archives:
        raise ValueError(f"No native archives in {snapshot}")
    for archive in archives:
        found = False
        for filename, data in native_files(snapshot, archive):
            name = Path(filename).name
            target = destination / archive.stem / name
            target.parent.mkdir(parents=True, exist_ok=True)
            if target.exists():
                raise ValueError(f"Duplicate native file in {archive}: {name}")
            target.write_bytes(data)
            if architecture and name.endswith((".dylib", ".dylib.debug")):
                arches = subprocess.check_output(
                    [llvm_tool("llvm-lipo"), "-archs", str(target)],
                    text=True,
                    timeout=120,
                ).split()
                if architecture not in arches:
                    raise ValueError(
                        f"Missing {architecture} slice in {archive.name}/{name}"
                    )
                thin = target.with_name(target.name + ".thin")
                if len(arches) > 1:
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
                    thin.replace(target)
            if name.endswith((".so", ".dll", ".dylib")):
                format_of(target)
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


def compare_native(
    base, head, output, abidiff, vendors=None, report=None, architecture=None
):
    with tempfile.TemporaryDirectory(prefix="native-", dir=output) as directory:
        old = native_libraries(base, Path(directory) / "base", architecture)
        new = native_libraries(head, Path(directory) / "head", architecture)
        results = compare_native_libraries(old, new, output, abidiff)
        if report is not None:
            try:
                report["jni"] = compatibility_jni.compare(
                    base,
                    head,
                    old,
                    new,
                    vendors["native"] if vendors else [],
                    Path(directory),
                    architecture,
                )
                report["errors"].extend(report["jni"]["errors"])
            except (
                OSError,
                ValueError,
                zipfile.BadZipFile,
                subprocess.SubprocessError,
            ) as error:
                report["errors"].append(f"JNI: {error}")
        if vendors is not None:
            try:
                vendors["report"]["native"] = compatibility_native.trace_native(
                    vendors["native"],
                    old,
                    new,
                    results,
                    output,
                    Path(directory),
                    architecture,
                )
            except (OSError, ValueError, subprocess.SubprocessError) as error:
                vendors["report"]["native"] = {
                    "errors": [f"Native vendor tracing: {error}"]
                }
        return results


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
        print(f"Native: {name}", flush=True)
        log = output / (name.replace("/", "-") + ".txt")
        if name not in new:
            status = "removed"
            log.write_text(f"Published library removed: {name}\n", encoding="utf-8")
        elif name not in old:
            status = "added"
            log.write_text(f"Published library added: {name}\n", encoding="utf-8")
        elif format_of(old[name]) != "elf" or format_of(new[name]) != "elf":
            status = compare_debug_types(old[name], new[name], log)
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
                    "--redundant",
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


def compare_debug_types(old, new, log):
    """Compare exports and debug-record layouts for platforms unsupported by abidiff."""
    snapshots = []
    for label, path in (("base", old), ("head", new)):
        data = log.with_suffix(f".{label}.json")
        diagnostic = log.with_suffix(f".{label}.txt")
        code = run_tool(
            [
                sys.executable,
                str(Path(__file__).with_name("compatibility_layouts.py")),
                str(path),
                str(data),
            ],
            diagnostic,
        )
        if code:
            log.write_text(
                f"Debug-type inspection failed ({code}); see {diagnostic.name}\n"
            )
            return "error"
        snapshots.append(json.loads(data.read_text()))
    before, after = snapshots
    lines = []
    for symbol in sorted(set(before["exports"]) - set(after["exports"])):
        lines.append(f"Removed export: {symbol}")
    for name in sorted(before["records"].keys() & after["records"].keys()):
        if before["records"][name] != after["records"][name]:
            lines += [
                f"'class {name}' changed:",
                "  before: " + json.dumps(before["records"][name]),
                "  after: " + json.dumps(after["records"][name]),
            ]
    for name in sorted(before["functions"].keys() & after["functions"].keys()):
        if before["functions"][name] != after["functions"][name]:
            lines.append(
                f"Function signature changed: {name}: {before['functions'][name]} -> {after['functions'][name]}"
            )
    for name in sorted(before["variables"].keys() & after["variables"].keys()):
        if before["variables"][name] != after["variables"][name]:
            lines.append(
                f"Variable type changed: {name}: {before['variables'][name]} -> {after['variables'][name]}"
            )
    for name, method in before["virtual_methods"].items():
        if method != after["virtual_methods"].get(name):
            lines += [
                f"'class {method['owner']}' changed: virtual method ABI",
                f"Virtual method ABI changed: {name}",
            ]
    log.write_text(
        "\n".join(lines) + "\n"
        if lines
        else "No exported symbol or debug-type changes.\n"
    )
    return "changed" if lines else "unchanged"


def prepare_macos(snapshot):
    """Link DWARF while the build machine's object files still exist."""
    for archive in sorted((snapshot / "native").glob("*.zip")):
        with tempfile.TemporaryDirectory() as directory:
            replacement = Path(directory) / "archive.zip"
            with (
                zipfile.ZipFile(archive) as source,
                zipfile.ZipFile(replacement, "w", zipfile.ZIP_DEFLATED) as dest,
            ):
                for entry in source.infolist():
                    if entry.filename.endswith(".dylib.debug"):
                        continue
                    # writestr updates the ZipInfo offset; read before copying it.
                    data = source.read(entry)
                    dest.writestr(entry, data)
                    if entry.filename.endswith(".dylib"):
                        binary = Path(directory) / Path(entry.filename).name
                        binary.write_bytes(data)
                        debug = Path(str(binary) + ".debug")
                        subprocess.run(
                            [
                                llvm_tool("dsymutil"),
                                "--flat",
                                "-o",
                                str(debug),
                                str(binary),
                            ],
                            check=True,
                            timeout=1200,
                        )
                        dest.write(debug, entry.filename + ".debug")
            archive.write_bytes(replacement.read_bytes())


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
        (
            "Scope: published Java definitions, native libraries (including JNI and HAL/GUI "
            "simulation plugins), native debug types, and JNI contracts. "
            f"Target: {html.escape(str(report.get('target', 'linuxx86-64 release')))}."
        ),
        (
            "These are potential client/vendor breakages. "
            "A clean report does not establish compatibility on other platforms or for "
            "header-only C++ code, inline behavior, reflection, or other runtime semantics."
        ),
        "",
    ]
    if report.get("baseline_warning"):
        lines += [
            f"**Baseline warning:** {html.escape(report['baseline_warning'])}",
            "",
        ]
    if report["errors"]:
        lines += ["**INCOMPLETE: some checks could not run.**", ""]
        lines += [f"- {html.escape(error)}" for error in report["errors"]]
        lines.append("")
    if report.get("warnings"):
        lines += ["**INCOMPLETE vendor coverage (warnings):**", ""]
        lines += [f"- {html.escape(warning)}" for warning in report["warnings"]]
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
    vendors = report.get("vendors")
    if vendors is not None:
        lines += ["", "## Vendordep usage", ""]
        lines.append(
            f"Inventory: `{html.escape(vendors.get('series') or 'unavailable')}` at "
            f"`{html.escape(vendors.get('revision') or 'unavailable')}`. "
            f"{vendors.get('downloaded', 0)}/{vendors.get('requested', 0)} artifacts downloaded; "
            f"{vendors.get('skipped', 0)} dependencies have no binary declared for this target."
        )
        lines += [
            "",
            (
                "References are static evidence, not proof that a code path executes. "
                "C++ type-name matches are conservative leads for review; "
                "missing matches do not establish compatibility."
            ),
            "",
        ]
        for language in ("native", "java"):
            usage = vendors.get(language, {})
            findings = usage.get("findings", [])
            lines.append(
                f"### {language.title()}: {len(findings)} consumers with references ({usage.get('scanned', 0)} artifacts scanned)"
            )
            lines.append("")
            for finding in findings[:50]:
                names = ", ".join(finding["manifests"])
                caller = finding.get("class", finding["entry"])
                lines.append(f"- {html.escape(names)} — `{html.escape(caller)}`")
                targets = []
                for reference in finding["references"]:
                    if "target" in reference:
                        targets.append(
                            reference["target"]
                            + " ("
                            + ", ".join(reference["changes"])
                            + ")"
                        )
                    elif "symbol" in reference:
                        targets.append("removed symbol: " + reference["symbol"])
                    else:
                        evidence = sorted(
                            {
                                m.get("type", m["demangled"])
                                for m in reference["matches"]
                            }
                        )
                        targets.append(
                            reference["library"]
                            + ": "
                            + ", ".join(e[:180] for e in evidence[:3])
                            + " ("
                            + reference["report"]
                            + ")"
                        )
                lines.append("  " + html.escape("; ".join(targets[:8])))
            if len(findings) > 50:
                lines.append("- Further consumers are in results.json.")
            lines.append("")
    else:
        lines += ["", "Vendordep tracing was not requested.", ""]
    jni = report.get("jni", {})
    lines += ["", f"## JNI: {len(jni.get('findings', []))} findings", ""]
    lines.append(jni.get("coverage", "JNI inspection did not complete."))
    if "native_declarations" in jni:
        lines.append(
            f"{jni['exported_bindings']} of {jni['native_declarations']} native declarations match exported JNI names; remaining declarations may use dynamic registration."
        )
    for finding in jni.get("findings", [])[:50]:
        lines.append("- " + html.escape(json.dumps(finding)[:2000]))
    if len(jni.get("findings", [])) > 50:
        lines.append("Further JNI findings are in results.json.")
    lines += [
        "",
        (
            "Download the compatibility artifact for full C++ diagnostics, Java HTML/XML, "
            "and machine-readable results.json. Findings do not block merging."
        ),
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
        "errors": list(getattr(args, "setup_errors", [])),
        "warnings": [],
        "requested_base": (getattr(args, "requested_base", None) or args.base_revision),
        "baseline_warning": getattr(args, "baseline_warning", ""),
    }
    architecture = getattr(args, "architecture", None)
    target = {"platform": "linuxx86-64", "configuration": "release"}
    target_error = None
    try:
        targets = [
            (
                json.loads((snapshot / "target.json").read_text())
                if (snapshot / "target.json").is_file()
                else target
            )
            for snapshot in (args.base, args.head)
        ]
        if targets[0] != targets[1]:
            raise ValueError("Baseline and candidate target metadata differ")
        target = targets[0]
        if (
            not isinstance(target, dict)
            or not {"platform", "configuration"} <= target.keys()
        ):
            raise ValueError("Invalid snapshot target metadata")
        if (target["platform"] == "osxuniversal") != bool(architecture):
            raise ValueError(
                "--architecture is required for macOS and unsupported for other targets"
            )
    except (OSError, ValueError) as error:
        target_error = str(error)
    report["target"] = (
        dict(target, architecture=architecture) if isinstance(target, dict) else target
    )
    vendors = None
    if getattr(args, "vendors", None) is not None:
        report["vendors"] = {}
        try:
            index = compatibility_vendors.load_inventory(args.vendors, output)
            for key in ("platform", "configuration"):
                if target_error:
                    raise ValueError(target_error)
                if index.get(key, target[key]) != target[key]:
                    raise ValueError(f"Vendor {key} does not match snapshot target")
            report["errors"].extend(index["errors"])
            report["warnings"].extend(index.get("warnings", []))
            available = [a for a in index["artifacts"] if a["status"] == "downloaded"]
            report["vendors"].update(
                series=index.get("series"),
                revision=index.get("revision"),
                downloaded=len(available),
                requested=len(index["artifacts"]),
                skipped=len(index["skipped"]),
            )
            vendors = {"report": report["vendors"]}
            for kind in ("java", "native"):
                vendors[kind] = [
                    (a, args.vendors / "artifacts" / a["path"])
                    for a in available
                    if (
                        "java" in a["kinds"]
                        if kind == "java"
                        else bool(set(a["kinds"]) & {"cpp", "jni"})
                    )
                ]
        except (OSError, ValueError, KeyError) as error:
            report["errors"].append(f"Vendordeps: {error}")
    try:
        for snapshot in (args.base, args.head):
            if not (snapshot / "complete").is_file():
                raise ValueError(f"Snapshot did not finish: {snapshot}")
        try:
            if target_error:
                raise ValueError(target_error)
            report["native"] = compare_native(
                args.base,
                args.head,
                output,
                args.abidiff,
                vendors,
                report,
                architecture,
            )
            for result in report["native"]:
                if result["status"] == "error":
                    report["errors"].append(
                        f"Native ABI inspection failed: {result['library']}"
                    )
        except (
            OSError,
            ValueError,
            zipfile.BadZipFile,
            subprocess.SubprocessError,
        ) as error:
            report["errors"].append(f"C++: {error}")
        try:
            report["java"] = compare_java(args.base, args.head, output, args.japicmp)
            if vendors is not None:
                report["vendors"]["java"] = compatibility_java.trace_java(
                    vendors["java"], args.base, output / "java.xml"
                )
        except (OSError, ValueError, ET.ParseError, zipfile.BadZipFile) as error:
            report["errors"].append(f"Java: {error}")
    except ValueError as error:
        report["errors"].append(str(error))
    for usage in report.get("vendors", {}).values():
        if isinstance(usage, dict):
            report["errors"].extend(usage.get("errors", []))
            report["warnings"].extend(usage.get("warnings", []))
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
    diff.add_argument(
        "--architecture",
        choices=["x86_64", "arm64"],
        help="Required slice for macOS universal binaries",
    )
    diff.add_argument(
        "--vendors", type=Path, help="Output of download-vendors (optional locally)"
    )
    vendor = commands.add_parser("download-vendors")
    vendor.add_argument("--repo", type=Path, required=True)
    vendor.add_argument("--baseline", required=True)
    vendor.add_argument("--inventory", type=Path, required=True)
    vendor.add_argument(
        "--series", help="Override the series inferred from the baseline tag"
    )
    vendor.add_argument("--output", type=Path, required=True)
    vendor.add_argument(
        "--platform",
        default="linuxx86-64",
        choices=[
            "linuxx86-64",
            "linuxarm64",
            "linuxsystemcore",
            "windowsx86-64",
            "windowsarm64",
            "osxuniversal",
        ],
    )
    vendor.add_argument(
        "--configuration", default="release", choices=["release", "debug"]
    )
    prepare = commands.add_parser("prepare-macos")
    prepare.add_argument("--snapshot", type=Path, required=True)
    args = parser.parse_args()
    if args.command == "previous-tag":
        print(previous_tag(args.repo, args.head))
        return 0
    if args.command == "download-vendors":
        return compatibility_vendors.download(args)
    if args.command == "prepare-macos":
        prepare_macos(args.snapshot)
        return 0
    return compare(args)


if __name__ == "__main__":
    raise SystemExit(main())
