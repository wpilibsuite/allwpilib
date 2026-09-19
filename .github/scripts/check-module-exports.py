#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import os
import re
import sys
from pathlib import Path

# Subprojects that are not libraries and do not need to export packages
IGNORED_PROJECTS = {
    "developerRobot",
    "epilogue-processor",
    "wpilibjExamples",
}

# Directories to ignore when traversing the workspace
EXCLUDED_DIR_NAMES = {
    ".git",
    ".gradle",
    "build",
    "bin",
    "docs",
}

# Directory and file name patterns to ignore within a project's source tree (e.g. tests, dev entrypoints)
EXCLUDED_SRC_PARTS = {
    "test",
    "tests",
    "dev",
    "test-dev",
    "printlog",
    "module-info.java",
    "package-info.java",
}


def strip_comments(text: str) -> str:
    """Removes C-style single-line and multi-line comments from text."""
    return re.sub(r"/\*.*?\*/|//.*?$", "", text, flags=re.DOTALL | re.MULTILINE)


def parse_module_info(module_info_path: Path) -> tuple[str, set[str]]:
    """Extracts module name and exported packages from a module-info.java file."""
    try:
        content = module_info_path.read_text(encoding="utf-8")
    except OSError as e:
        print(f"Warning: Failed to read {module_info_path}: {e}", file=sys.stderr)
        return "unknown", set()

    clean_content = strip_comments(content)
    module_match = re.search(r"\b(?:open\s+)?module\s+([a-zA-Z0-9_.]+)", clean_content)
    module_name = module_match.group(1) if module_match else "unknown"

    # Matches `exports <package_name>;` as well as qualified `exports <package_name> to ...;`
    exports = set(
        re.findall(
            r"exports\s+([a-zA-Z0-9_.]+)(?:\s+to\s+[^;]+)?\s*;",
            clean_content,
        )
    )
    return module_name, exports


def get_declared_packages(project_dir: Path) -> set[str]:
    """
    Scans Java source files in a subproject and returns all declared packages.
    Internal packages (.internal) and their subpackages are not included in the returned set.
    """
    declared_packages = set()
    src_dir = project_dir / "src"
    if not src_dir.exists():
        return declared_packages

    for java_file in src_dir.glob("**/*.java"):
        # Exclude tests, dev runners, and other files that aren't part of the library classes
        rel_parts = set(java_file.relative_to(src_dir).parts)
        if rel_parts & EXCLUDED_SRC_PARTS:
            continue

        try:
            with open(java_file, "r", encoding="utf-8", errors="ignore") as f:
                for line in f:
                    line = line.strip()
                    if line.startswith("package ") and line.endswith(";"):
                        pkg = line[len("package ") : -1].strip()
                        if pkg and not (pkg.endswith(".internal") or ".internal." in pkg):
                            # Add this package unless it's internal (including subpackages of internal packages).
                            # They can still be exported, but won't be treated as required.
                            declared_packages.add(pkg)
                        break
        except OSError as e:
            print(f"Warning: Failed to read {java_file}: {e}", file=sys.stderr)

    return declared_packages


def check_subproject(module_info_path: Path, root_dir: Path) -> tuple[str, str, list[str]]:
    """
    Checks if a subproject exports all of its declared packages.
    Internal packages are not required for the check to pass, though projects may still decide to export them.
    Returns (project_name, module_name, list_of_missing_packages).
    """
    # Determine project directory containing src/
    parts = module_info_path.parts
    if "src" in parts:
        src_idx = parts.index("src")
        project_dir = Path(*parts[:src_idx]) if src_idx > 0 else Path(".")
    else:
        project_dir = module_info_path.parent

    try:
        project_name = str(project_dir.resolve().relative_to(root_dir.resolve()))
    except ValueError:
        project_name = str(project_dir)

    module_name, exported = parse_module_info(module_info_path)
    declared = get_declared_packages(project_dir)

    missing = sorted(declared - exported)
    return project_name, module_name, missing


def find_module_info_files(root_dir: Path) -> list[Path]:
    """Finds all module-info.java files in the repository, excluding ignored projects/dirs."""
    module_info_files = []
    for path in root_dir.glob("**/src/main/java/module-info.java"):
        parts = path.parts
        # Skip bazel and gradle build outputs, excluded directories
        if any(
            p.startswith("bazel-") or p in EXCLUDED_DIR_NAMES
            for p in parts
        ):
            continue
        # Skip explicitly ignored projects
        if any(ignored in parts for ignored in IGNORED_PROJECTS):
            continue

        module_info_files.append(path)

    return sorted(module_info_files)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Verify that all Java packages in subprojects are exported in module-info.java"
    )
    parser.add_argument(
        "--root",
        type=Path,
        default=Path.cwd(),
        help="Root directory of the repository (default: current working directory)",
    )
    args = parser.parse_args()

    root_dir = args.root.resolve()
    module_info_files = find_module_info_files(root_dir)

    if not module_info_files:
        print("No module-info.java files found to check.", file=sys.stderr)
        return 1

    failures: list[tuple[str, str, list[str]]] = []
    checked_count = 0

    for module_info_path in module_info_files:
        project_name, module_name, missing_packages = check_subproject(module_info_path, root_dir)
        checked_count += 1
        if missing_packages:
            failures.append((project_name, module_name, missing_packages))

    if failures:
        print("=" * 70, file=sys.stderr)
        print("ERROR: Java packages are missing from module-info.java exports!", file=sys.stderr)
        print("       (note: `.internal` packages are not required to be exported)", file=sys.stderr)
        print("=" * 70, file=sys.stderr)
        failures.sort(key=lambda x: (x[0], x[1]))
        for proj, mod, pkgs in failures:
            print(f"\nModule '{mod}' in project '{proj}' is missing exports for {len(pkgs)} package(s):", file=sys.stderr)
            for pkg in pkgs:
                print(f"  - {pkg}", file=sys.stderr)
        print("\nPlease add the missing 'exports <package>;' directives to their respective module-info.java files.\n", file=sys.stderr)
        return 1

    print(f"Success: All {checked_count} subprojects export their declared Java packages.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
