#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Reuse normal Gradle CI artifacts for compatibility reports."""

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
import zipfile
from pathlib import Path

import compatibility
import compatibility_artifactory
from compatibility_binary import llvm_tool

TARGETS = [
    {"platform": "linuxx86-64", "artifacts": ["Linux"]},
    {"platform": "linuxarm64", "artifacts": ["Arm64"]},
    {"platform": "linuxsystemcore", "artifacts": ["Systemcore"]},
    {"platform": "windowsx86-64", "artifacts": ["Win64Debug", "Win64Release"]},
    {"platform": "windowsarm64", "artifacts": ["WinArm64Debug", "WinArm64Release"]},
    {"platform": "osxuniversal", "artifacts": ["macOS"]},
]


def plan(args):
    targets = compatibility_artifactory.select(
        args.repo, args.revision, args.candidate, args.branch, TARGETS
    )
    for target in targets:
        if target["baseline_error"]:
            print(target["baseline_error"] + " No rebuild will be attempted.")
        else:
            print(
                f"{target['platform']}: Artifactory {target['baseline_repository']}/{target['baseline_version']}"
            )
    with args.output.open("a") as output:
        output.write(f"matrix={json.dumps({'include': targets})}\n")


def filename(value):
    """Manifest paths are data, not permission to read outside the artifact."""
    if (
        not isinstance(value, str)
        or not value
        or value in (".", "..")
        or any(c in value for c in "/\\:")
    ):
        raise ValueError(f"Invalid artifact filename: {value!r}")
    return value


def read_manifest(path):
    manifest = json.loads(path.read_text())
    if manifest.get("schema") != 1 or not re.fullmatch(
        r"[0-9a-f]{40}", manifest.get("revision") or ""
    ):
        raise ValueError(f"Invalid artifact provenance: {path}")
    if manifest.get("platform") not in {t["platform"] for t in TARGETS}:
        raise ValueError(f"Invalid artifact platform: {path}")
    targets = manifest.get("targets", {})
    if not targets or set(targets) - {"release", "debug"}:
        raise ValueError(f"Invalid artifact configurations: {path}")
    for target in targets.values():
        for kind, suffix in (("native", ".zip"), ("java", ".jar")):
            entries = target.get(kind, [])
            names = set()
            if not entries:
                raise ValueError(f"Empty {kind} inventory: {path}")
            for entry in entries:
                for key in ("file", "name"):
                    if not filename(entry[key]).endswith(suffix):
                        raise ValueError(f"Invalid {kind} filename: {entry[key]}")
                if entry["name"] in names:
                    raise ValueError(f"Duplicate {kind} name: {entry['name']}")
                names.add(entry["name"])
    return manifest


def prepare(products):
    """Finish supplemental data while macOS's original object files still exist."""
    extras = products / "compatibility"
    complete = extras / "complete"
    complete.unlink(missing_ok=True)
    manifest = read_manifest(extras / "manifest.json")
    for configuration, target in manifest["targets"].items():
        for kind in ("java", "native"):
            for entry in target[kind]:
                archive = products / entry["file"]
                if not archive.is_file():
                    raise ValueError(f"Missing published artifact: {archive}")
                if kind != "native" or manifest["platform"] != "osxuniversal":
                    continue
                debug_dir = (
                    extras / configuration / "native-debug" / Path(entry["name"]).stem
                )
                debug_dir.mkdir(parents=True, exist_ok=True)
                with (
                    zipfile.ZipFile(archive) as source,
                    tempfile.TemporaryDirectory() as directory,
                ):
                    packaged = set(source.namelist())
                    for member in source.infolist():
                        if not member.filename.endswith(".dylib"):
                            continue
                        if member.filename + ".debug" in packaged:
                            continue
                        binary = Path(directory) / Path(member.filename).name
                        binary.write_bytes(source.read(member))
                        subprocess.run(
                            [
                                llvm_tool("dsymutil"),
                                "--flat",
                                "-o",
                                str(debug_dir / (binary.name + ".debug")),
                                str(binary),
                            ],
                            check=True,
                            timeout=1200,
                        )
    complete.write_text("complete\n")


def link_or_copy(source, destination):
    destination = Path(destination)
    destination.parent.mkdir(parents=True, exist_ok=True)
    try:
        os.link(source, destination)
    except OSError:
        shutil.copyfile(source, destination)


def find_manifest(products, revision, platform, configuration):
    """Require a complete artifact for the exact commit and target."""
    matches = []
    for path in products.rglob("compatibility/manifest.json"):
        manifest = read_manifest(path)
        if manifest["platform"] == platform and configuration in manifest["targets"]:
            matches.append((path, manifest))
    if len(matches) != 1:
        raise ValueError(
            f"Expected one compatibility/manifest.json for {platform} {configuration}; "
            f"found {len(matches)} in {products}. Artifact download or collection may be incomplete."
        )
    path, manifest = matches[0]
    if manifest["revision"] != revision:
        raise ValueError(
            f"Artifact revision {manifest['revision']} does not match {revision}"
        )
    extras = path.parent
    if not (extras / "complete").is_file():
        raise ValueError(f"Supplemental artifact data is incomplete: {extras}")
    for kind in ("java", "native"):
        for entry in manifest["targets"][configuration][kind]:
            if not (extras.parent / entry["file"]).is_file():
                raise ValueError(
                    f"Missing published artifact: {extras.parent / entry['file']}"
                )
    return path, manifest


def materialize(products, destination, revision, platform, configuration):
    """Present existing binaries to the analyzer without rebuilding or repackaging."""
    path, manifest = find_manifest(products, revision, platform, configuration)
    extras = path.parent
    for kind, entries in manifest["targets"][configuration].items():
        if kind not in ("java", "native"):
            continue
        for entry in entries:
            link_or_copy(
                extras.parent / entry["file"], destination / kind / entry["name"]
            )
    for source, name in (
        (extras / "classpath", "classpath"),
        (extras / configuration / "native-debug", "native-debug"),
    ):
        if source.is_dir():
            shutil.copytree(source, destination / name, copy_function=link_or_copy)
    (destination / "target.json").write_text(
        json.dumps({"platform": platform, "configuration": configuration})
    )
    (destination / "provenance.json").write_text(json.dumps(manifest, indent=2))
    (destination / "complete").write_text("complete\n")


def validate(args):
    """Fail missing prerequisites without invoking builds, downloads, or ABI tools."""
    errors = []
    baseline_error = getattr(args, "baseline_error", "")
    if baseline_error:
        errors.append(baseline_error)
    else:
        for side, products, revision in (
            ("Baseline", args.baseline_products, args.base_revision),
            ("Candidate", args.candidate_products, args.head_revision),
        ):
            for configuration in ("release", "debug"):
                try:
                    find_manifest(products, revision, args.platform, configuration)
                except (OSError, ValueError, KeyError) as error:
                    errors.append(f"{side} {configuration}: {error}")
    if not errors:
        return 0
    result = {
        "base": args.base_label,
        "head": args.head_revision,
        "target": {"platform": args.platform},
        "native": [],
        "java": [],
        "errors": errors,
        "warnings": [],
        "requested_base": (getattr(args, "requested_base", None) or args.base_revision),
        "baseline_warning": getattr(args, "baseline_warning", ""),
    }
    args.output.mkdir(parents=True, exist_ok=True)
    (args.output / "results.json").write_text(json.dumps(result, indent=2) + "\n")
    (args.output / "summary.md").write_text(
        compatibility.summary(result) + "\nNo rebuild was attempted.\n"
    )
    print("\n".join(errors), file=sys.stderr)
    return 1


def report(args):
    """Run both configurations and every architecture, retaining partial reports."""
    if validate(args):
        return 1
    args.output.mkdir(parents=True, exist_ok=True)
    failed = False
    summaries = []
    with tempfile.TemporaryDirectory() as directory:
        root = Path(directory)
        for configuration in ("release", "debug"):
            print(f"Preparing {args.platform} {configuration}", flush=True)
            base = root / configuration / "baseline"
            head = root / configuration / "candidate"
            setup_errors = []
            for products, destination, revision in (
                (args.candidate_products, head, args.head_revision),
                (args.baseline_products, base, args.base_revision),
            ):
                try:
                    materialize(
                        products, destination, revision, args.platform, configuration
                    )
                except (OSError, ValueError, KeyError) as error:
                    setup_errors.append(str(error))
            vendors = root / configuration / "vendors"
            print(f"Downloading vendors: {args.platform} {configuration}", flush=True)
            download = subprocess.run(
                [
                    sys.executable,
                    str(Path(__file__).with_name("compatibility.py")),
                    "download-vendors",
                    "--repo",
                    str(args.repo),
                    "--baseline",
                    args.base_revision,
                    "--inventory",
                    str(args.inventory),
                    "--platform",
                    args.platform,
                    "--configuration",
                    configuration,
                    "--output",
                    str(vendors),
                ],
                check=False,
            )
            failed |= download.returncode != 0
            architectures = (
                ("x86_64", "arm64") if args.platform == "osxuniversal" else (None,)
            )
            for architecture in architectures:
                print(
                    f"Comparing {args.platform} {configuration} {architecture or ''}",
                    flush=True,
                )
                output = args.output / configuration / (architecture or "native")
                output.mkdir(parents=True, exist_ok=True)
                if setup_errors:
                    (output / "setup-errors.txt").write_text(
                        "\n".join(setup_errors) + "\n"
                    )
                    print("\n".join(setup_errors), flush=True)
                    failed = True
                code = compatibility.compare(
                    argparse.Namespace(
                        base=base,
                        head=head,
                        output=output,
                        base_revision=args.base_label,
                        head_revision=args.head_revision,
                        abidiff="abidiff",
                        japicmp=args.japicmp,
                        vendors=vendors,
                        architecture=architecture,
                        setup_errors=setup_errors,
                        requested_base=(
                            getattr(args, "requested_base", None) or args.base_revision
                        ),
                        baseline_warning=getattr(args, "baseline_warning", ""),
                    )
                )
                failed |= code != 0
                summaries.append(
                    f"## {args.platform} {configuration} {architecture or ''}\n\n"
                    + (output / "summary.md").read_text()
                )
    (args.output / "summary.md").write_text("\n\n".join(summaries))
    return int(failed)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    planner = commands.add_parser("plan")
    planner.add_argument("--repo", type=Path, required=True)
    planner.add_argument("--revision", required=True)
    planner.add_argument("--candidate", required=True)
    planner.add_argument("--branch", required=True)
    planner.add_argument("--output", type=Path, required=True)
    producer = commands.add_parser("prepare")
    producer.add_argument("--products", type=Path, required=True)
    resolver = commands.add_parser("resolve-classpath")
    resolver.add_argument("--products", type=Path, required=True)
    resolver.add_argument("--log", type=Path, required=True)
    for command in ("validate", "fetch-baseline", "report"):
        reports = commands.add_parser(command)
        for name in ("candidate-products", "baseline-products", "output"):
            reports.add_argument("--" + name, type=Path, required=True)
        for name in ("platform", "base-revision", "base-label", "head-revision"):
            reports.add_argument("--" + name, required=True)
        reports.add_argument("--requested-base")
        reports.add_argument("--baseline-warning", default="")
        if command == "validate":
            reports.add_argument("--baseline-error", default="")
        elif command == "fetch-baseline":
            reports.add_argument("--repo", type=Path, required=True)
            reports.add_argument("--version", required=True)
            reports.add_argument("--repository", required=True)
        else:
            for name in ("repo", "inventory", "japicmp"):
                reports.add_argument("--" + name, type=Path, required=True)
    args = parser.parse_args()
    if args.command == "plan":
        plan(args)
    elif args.command == "prepare":
        prepare(args.products)
    elif args.command == "fetch-baseline":
        try:
            compatibility_artifactory.download(
                args.repo,
                {
                    "revision": args.base_revision,
                    "version": args.version,
                    "repository": args.repository,
                },
                args.platform,
                args.baseline_products,
            )
        except (OSError, ValueError, KeyError, subprocess.SubprocessError) as error:
            args.baseline_error = f"Artifactory baseline download failed: {error}"
            return validate(args)
    elif args.command == "resolve-classpath":
        compatibility_artifactory.resolve_classpath(args.products, args.log)
    elif args.command == "validate":
        return validate(args)
    else:
        return report(args)
    return 0


if __name__ == "__main__":
    sys.exit(main())
