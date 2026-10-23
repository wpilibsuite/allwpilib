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
import urllib.parse
import urllib.request
import zipfile
from pathlib import Path

import compatibility
from compatibility_binary import llvm_tool

TARGETS = [
    {"platform": "linuxx86-64", "artifacts": ["Linux"]},
    {"platform": "linuxarm64", "artifacts": ["Arm64"]},
    {"platform": "linuxsystemcore", "artifacts": ["Systemcore"]},
    {"platform": "windowsx86-64", "artifacts": ["Win64Debug", "Win64Release"]},
    {"platform": "windowsarm64", "artifacts": ["WinArm64Debug", "WinArm64Release"]},
    {"platform": "osxuniversal", "artifacts": ["macOS"]},
]


def api_pages(repository, endpoint, key, **query):
    """Read paginated Actions metadata; credentials are sent only to GitHub."""
    for page in range(1, 11):
        url = f"https://api.github.com/repos/{repository}/actions/{endpoint}?"
        url += urllib.parse.urlencode({**query, "per_page": 100, "page": page})
        request = urllib.request.Request(
            url,
            headers={
                "Accept": "application/vnd.github+json",
                "Authorization": f"Bearer {os.environ['GITHUB_TOKEN']}",
                "X-GitHub-Api-Version": "2022-11-28",
            },
        )
        with urllib.request.urlopen(request, timeout=60) as response:
            items = json.load(response)[key]
        yield from items
        if len(items) < 100:
            break


def artifact_plan(
    revision, runs, artifacts, *, fallback_runs=lambda: (), branch="", candidate=""
):
    """Prefer the exact baseline, then recent usable push artifacts on its branch."""
    targets = [
        dict(target, baseline_run="", baseline_revision=revision, baseline_warning="")
        for target in TARGETS
    ]
    seen = set()
    for exact in (True, False):
        for run in runs if exact else fallback_runs():
            if (
                run["event"] != "push"
                or run["head_sha"] == candidate
                or run["id"] in seen
                or (exact and run["head_sha"] != revision)
                or (not exact and run["head_branch"] != branch)
            ):
                continue
            seen.add(run["id"])
            names = {a["name"] for a in artifacts(run) if not a["expired"]}
            for target in targets:
                if not target["baseline_run"] and set(target["artifacts"]) <= names:
                    target["baseline_run"] = str(run["id"])
                    target["baseline_revision"] = run["head_sha"]
                    if run["head_sha"] != revision:
                        target["baseline_warning"] = (
                            f"Requested baseline {revision} has no usable {target['platform']} artifacts. "
                            f"Using {run['head_sha']} from Gradle run {run['id']}, the most recent "
                            f"usable artifacts on {branch}; the comparison baseline has changed."
                        )
            if all(t["baseline_run"] for t in targets):
                break
        if all(t["baseline_run"] for t in targets):
            break
    for target in targets:
        names = target["artifacts"]
        target["pattern"] = names[0] if len(names) == 1 else "{" + ",".join(names) + "}"
    return targets


def plan(args):
    supported = {}

    def available(run):
        revision = run["head_sha"]
        if revision not in supported:
            supported[revision] = (
                subprocess.run(
                    [
                        "git",
                        "-C",
                        str(args.repo),
                        "cat-file",
                        "-e",
                        f"{revision}:.github/scripts/compatibility_artifacts.py",
                    ],
                    stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL,
                    check=False,
                ).returncode
                == 0
            )
        if not supported[revision]:
            if revision == args.revision:
                print(
                    f"Baseline Gradle run {run['id']} ({revision}) predates compatibility artifact data."
                )
            return []
        # Artifact names alone do not establish that supplemental data was produced.
        jobs = api_pages(args.repository, f"runs/{run['id']}/jobs", "jobs")
        ready = {
            job["name"].removeprefix("Build - ")
            for job in jobs
            if job["name"].startswith("Build - ")
            and any(
                step["name"] == "Add compatibility data to build artifacts"
                and step["conclusion"] == "success"
                for step in (job.get("steps") or [])
            )
        }
        return [
            artifact
            for artifact in api_pages(
                args.repository, f"runs/{run['id']}/artifacts", "artifacts"
            )
            if artifact["name"] in ready
        ]

    lookup_error = ""
    try:
        targets = artifact_plan(
            args.revision,
            api_pages(
                args.repository,
                "workflows/gradle.yml/runs",
                "workflow_runs",
                head_sha=args.revision,
                event="push",
            ),
            available,
            fallback_runs=lambda: api_pages(
                args.repository,
                "workflows/gradle.yml/runs",
                "workflow_runs",
                branch=args.branch,
                event="push",
            ),
            branch=args.branch,
            candidate=args.candidate,
        )
    except (OSError, ValueError) as error:
        lookup_error = f"Cannot look up baseline Gradle artifacts: {error}"
        targets = artifact_plan(args.revision, [], lambda _: [])
    for target in targets:
        error = lookup_error
        if not error and not target["baseline_run"]:
            error = (
                f"No usable Gradle artifacts ({', '.join(target['artifacts'])}) for baseline "
                f"{args.revision} or recent push builds on {args.branch}. "
                "Artifacts must include compatibility metadata, Java dependency classpaths, "
                "and native debug data; the candidate cannot be its own baseline."
            )
        target["baseline_error"] = error
        if error:
            print(f"{target['platform']}: {error} No rebuild will be attempted.")
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
                    for member in source.infolist():
                        if not member.filename.endswith(".dylib"):
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
            f"found {len(matches)} in {products}. Build artifacts may be missing or predate compatibility data."
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
    planner.add_argument("--repository", required=True)
    planner.add_argument("--output", type=Path, required=True)
    producer = commands.add_parser("prepare")
    producer.add_argument("--products", type=Path, required=True)
    for command in ("validate", "report"):
        reports = commands.add_parser(command)
        for name in ("candidate-products", "baseline-products", "output"):
            reports.add_argument("--" + name, type=Path, required=True)
        for name in ("platform", "base-revision", "base-label", "head-revision"):
            reports.add_argument("--" + name, required=True)
        reports.add_argument("--requested-base")
        reports.add_argument("--baseline-warning", default="")
        if command == "validate":
            reports.add_argument("--baseline-error", default="")
        else:
            for name in ("repo", "inventory", "japicmp"):
                reports.add_argument("--" + name, type=Path, required=True)
    args = parser.parse_args()
    if args.command == "plan":
        plan(args)
    elif args.command == "prepare":
        prepare(args.products)
    elif args.command == "validate":
        return validate(args)
    else:
        return report(args)
    return 0


if __name__ == "__main__":
    sys.exit(main())
