#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import re
import sys
from pathlib import Path

# Parses Java files in a subproject and replaces all imports of top-level WPILib classes
# with module imports for the modules that export the packages those classes belong to.
# If the subproject depends on the allwpilib-java project, module imports will use the `wpilib`
# module for everything under the umbrella instead of per-module imports like `wpilib.core` or
# `wpilib.math`.
#
# This script is idempotent. Only library source files will be updated; generated sources,
# templates, and test files will not be touched.
#
# Usage:
# ./scripts/update-to-module-imports.py --project <project>

# Directories to ignore when traversing the workspace
EXCLUDED_DIR_NAMES = {
    ".git",
    ".gradle",
    "build",
    "bin",
    "docs",
}

# Types that we should NOT update to module imports due to identical class names appearing
# in more than one package.
AMBIGUOUS_TYPES = {
    "org.wpilib.math.estimator.KalmanFilter",  # conflicts with org.opencv.video.KalmanFilter
}


def find_module_info_files() -> list[Path]:
    """Finds all module-info.java files in the repository, excluding ignored projects/dirs."""
    module_info_files = []
    for path in Path.cwd().glob("**/src/main/java/module-info.java"):
        path = path.relative_to(Path.cwd())
        parts = path.parts
        # Skip bazel and gradle build outputs, excluded directories
        if any(p.startswith("bazel-") or p in EXCLUDED_DIR_NAMES for p in parts):
            continue

        module_info_files.append(path)

    return sorted(module_info_files)


def strip_comments(text: str) -> str:
    """Removes C-style single-line and multi-line comments from text."""
    return re.sub(r"/\*.*?\*/|//.*?$", "", text, flags=re.DOTALL | re.MULTILINE)


def parse_module_info(module_info_path: Path) -> tuple[str, set[str], set[str]]:
    """
    Extracts module name, exported packages, and transitive requirements from a module-info.java file.
    """
    try:
        content = module_info_path.read_text(encoding="utf-8")
    except OSError as e:
        print(f"Warning: Failed to read {module_info_path}: {e}", file=sys.stderr)
        return {
            "name": "unknown",
            "exports": set(),
            "requirements": set(),
            "transitive_requirements": set(),
        }

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

    # Matches `requires transitive <module_name>;`
    transitive_requirements = set(
        re.findall(r"requires transitive\s+([a-zA-Z0-9_.]+)\s*;", clean_content)
    )
    requirements = set(re.findall(r"requires\s+([a-zA-Z0-9_.]+)\s*;", clean_content))

    return {
        "name": module_name,
        "exports": exports,
        "requirements": requirements,
        "transitive_requirements": transitive_requirements,
    }


def reorder_and_dedup_imports(content):
    """
    Reorders and deduplicates import statements from Java file content.
    Import statements will be ordered into groups, and elements within each group ordered lexicographically.
    Groups are ordered to match the same order output by google-java-format:

    1. `import static ...`
    2. `import module ...`
    3. `import ...`
    """
    static_import_pattern = re.compile("import static .*;")
    module_import_pattern = re.compile("import module .*;")
    standard_import_pattern = re.compile("import .*;")

    static_imports = set()
    module_imports = set()
    standard_imports = set()

    lines = content.split("\n")

    first_import_line = None
    last_import_line = None

    # Scan the text contents line-by-line to group imports by type and find the chunk dedicated to
    # import statements. Note that any custom blank lines and commented-out lines will be removed.
    lineno = -1
    for line in lines:
        lineno = lineno + 1
        if standard_import_pattern.search(line):
            if first_import_line is None:
                last_import_line = first_import_line = lineno
            else:
                last_import_line = lineno

            if static_import_pattern.search(line):
                static_imports.add(line)
            elif module_import_pattern.search(line):
                module_imports.add(line)
            else:
                standard_imports.add(line)

    # Replace the imports block with the standard format: module imports > static imports > standard imports.
    if first_import_line and last_import_line:
        new_imports = []

        # Custom sort that excludes the trailing semicolons from the lexicographic sort.
        # Otherwise `import module wpilib;` would appear after `import module wpilib.units;` (eg)
        # because the lexicographic order assigns the semicolon character `;` 59 and the period character `.` 46.
        content_sort = lambda line: line[0:-2]

        for group in [static_imports, module_imports, standard_imports]:
            if not group:
                # Skip empty groups
                continue
            if new_imports:
                # Ensure a blank line separates the groups
                new_imports.append("")
            # Sort the group lexicographically
            group = list(group)
            group.sort(key=content_sort)
            new_imports.extend(group)

        lines[first_import_line : last_import_line + 1] = new_imports

    return "\n".join(lines)


def main():
    module_info_files = find_module_info_files()

    if not module_info_files:
        print(
            "Did not find any module information. Ensure the working directory is the root allwpilib directory."
        )
        return 1

    parser = argparse.ArgumentParser(
        description="Updates a project to use WPILib module imports. Imports for modular dependencies like OpenCV will not be changed."
    )
    parser.add_argument(
        "--project",
        type=Path,
        required=True,
        help="The subproject to update",
    )
    args = parser.parse_args()
    target_dir = args.project.resolve()

    modules_by_package: dict = {}
    transitive_modules: dict = {}

    # Track which packages are exported by which modules
    for module_info_path in module_info_files:
        module_info = parse_module_info(module_info_path)
        for export in module_info["exports"]:
            modules_by_package[export] = module_info["name"]

        transitive_modules[module_info["name"]] = module_info["transitive_requirements"]

    target_mod_info = parse_module_info(target_dir / "src/main/java/module-info.java")

    # Special case the umbrella project so we have a single `import module wpilib;` statement
    # instead of multiple `import module wpilib.<x>` statements for each subproject under the
    # umbrella.
    if "wpilib" in target_mod_info["requirements"] and "wpilib" in transitive_modules:
        for pkg, mod in modules_by_package.items():
            if mod in transitive_modules["wpilib"]:
                modules_by_package[pkg] = "wpilib"

    for path in target_dir.glob("src/main/java/**/*.java"):
        try:
            content = path.read_text(encoding="utf-8")

            # 1. Replace traditional import statements with module imports.
            #    Inner classes, static imports, and existing module imports will be left as-is.
            new_content = re.sub(
                r"import ((.*)\..*);",
                # Replace the import statement with a module import statement, if the package the class is imported from
                # is present in the LUT. Otherwise replace with the source import; essentially a no-op
                lambda m: (
                    f"import module {modules_by_package.get(m.group(2))};"
                    if m.group(2) in modules_by_package
                    and not m.group(1) in AMBIGUOUS_TYPES
                    else m.group(0)
                ),
                content,
            )

            # 2. Sort and deduplicate import statements to static > module > standard ordering
            new_content = reorder_and_dedup_imports(new_content)

            if not new_content.endswith("\n"):
                # Ensure the final newline is still present
                new_content = new_content + "\n"

            path.write_text(new_content, encoding="utf-8")

            if new_content == content:
                print(f"No changes to {path}")
            else:
                print(f"Updated {path}")
        except OSError as e:
            print(f"Warning: Failed to read {path}: {e}", file=sys.stderr)

    return 0


if __name__ == "__main__":
    sys.exit(main())
