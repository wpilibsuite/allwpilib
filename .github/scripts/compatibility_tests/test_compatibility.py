# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Behavioral fixtures for the actual ABI tools, plus baseline/error handling tests."""

import argparse
import importlib.util
import json
import os
import shutil
import subprocess
import sys
import tempfile
import unittest
import zipfile
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[1] / "compatibility.py"
sys.path.insert(0, str(SCRIPT.parent))
SPEC = importlib.util.spec_from_file_location("compatibility", SCRIPT)
compatibility = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(compatibility)
JAPICMP = os.environ.get("JAPICMP_JAR")
ABIDIFF = os.environ.get("ABIDIFF", "abidiff")


def run(*command, cwd=None):
    return subprocess.check_output(
        command, cwd=cwd, text=True, stderr=subprocess.STDOUT
    )


class BaselineTests(unittest.TestCase):
    def test_previous_tag_follows_main_and_excludes_head(self):
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)

            def git(*args):
                return run("git", *args, cwd=repo).strip()

            git("init", "-b", "main")
            git("config", "user.name", "Compatibility Test")
            git("config", "user.email", "test@example.invalid")
            git("commit", "--allow-empty", "-m", "release")
            git("tag", "v2027.0.0-alpha-7")
            git("checkout", "-b", "feature")
            git("commit", "--allow-empty", "-m", "side branch")
            git("tag", "v2099.0.0")
            git("checkout", "main")
            git("commit", "--allow-empty", "-m", "main work")
            git("merge", "--no-ff", "feature", "-m", "merge")
            git("tag", "v2027.0.0-alpha-8")
            self.assertEqual(
                compatibility.previous_tag(repo, "HEAD"), "v2027.0.0-alpha-7"
            )
            git("commit", "--allow-empty", "-m", "next merge")
            self.assertEqual(
                compatibility.previous_tag(repo, "HEAD"), "v2027.0.0-alpha-8"
            )

    def test_missing_tag_is_an_error(self):
        with tempfile.TemporaryDirectory() as directory:
            run("git", "init", "-b", "main", cwd=directory)
            run("git", "config", "user.name", "Compatibility Test", cwd=directory)
            run("git", "config", "user.email", "test@example.invalid", cwd=directory)
            for message in ("initial", "next"):
                run("git", "commit", "--allow-empty", "-m", message, cwd=directory)
            with self.assertRaises(subprocess.CalledProcessError):
                compatibility.previous_tag(Path(directory), "HEAD")

    def test_abidiff_exit_codes(self):
        self.assertEqual(compatibility.native_status(0), "unchanged")
        for code in (4, 8, 12):
            self.assertEqual(compatibility.native_status(code), "changed")
        for code in (-1, 1, 2, 3, 5, 13, 16):
            self.assertEqual(compatibility.native_status(code), "error")

    def test_unfinished_snapshot_is_incomplete(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            args = argparse.Namespace(
                base=root / "base",
                head=root / "head",
                output=root / "report",
                base_revision="old",
                head_revision="new",
                abidiff=ABIDIFF,
                japicmp=Path("missing.jar"),
            )
            self.assertEqual(compatibility.compare(args), 1)
            self.assertIn("INCOMPLETE", (args.output / "summary.md").read_text())


@unittest.skipUnless(
    JAPICMP and shutil.which(ABIDIFF) and shutil.which("g++") and shutil.which("javac"),
    "Set JAPICMP_JAR and install abidiff, g++, and a JDK to run integration fixtures",
)
class ToolTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.base = self.root / "base"
        self.head = self.root / "head"
        self.output = self.root / "report"
        self.output.mkdir()
        for side in (self.base, self.head):
            (side / "java").mkdir(parents=True)
            (side / "native").mkdir()
            (side / "complete").touch()

    def native(self, snapshot, changed=False, debug=True, split_debug=False):
        # Layout keeps the same sizeof, while Callback grows; symbol names stay unchanged.
        fields = "int second; int first;" if changed else "int first; int second;"
        size = 4 if changed else 3
        source = snapshot / "api.cpp"
        source.write_text(
            f"struct Layout {{ {fields} }};\n"
            f"struct Callback {{ void* storage[{size}]; }};\n"
            "int consume(Layout* p, Callback* cb) { return p->first + (cb != nullptr); }\n"
        )
        library = snapshot / "libapi.so"
        command = ["g++", "-shared", "-fPIC", "-o", str(library), str(source)]
        if debug:
            command.append("-g")
        run(*command)
        if split_debug:
            run(
                "objcopy",
                "--only-keep-debug",
                "libapi.so",
                "libapi.so.debug",
                cwd=snapshot,
            )
            run(
                "objcopy",
                "--strip-debug",
                "--add-gnu-debuglink=libapi.so.debug",
                "libapi.so",
                cwd=snapshot,
            )
        with zipfile.ZipFile(snapshot / "native/api.zip", "w") as archive:
            archive.write(library, "linux/x86-64/shared/libapi.so")
            if split_debug:
                archive.write(
                    snapshot / "libapi.so.debug", "linux/x86-64/shared/libapi.so.debug"
                )

    def java(self, snapshot, changed=False):
        source = snapshot / "src"
        source.mkdir()
        support = snapshot / "support"
        support.mkdir()
        (source / "Base.java").write_text("package dep; public class Base {}")
        run("javac", "-d", str(support), str(source / "Base.java"))
        (snapshot / "classpath").mkdir()
        dependency = snapshot / "classpath/support.jar"
        run("jar", "cf", str(dependency), "-C", str(support), ".")
        method = "radians" if changed else "degrees"
        shape = "interface" if changed else "class"
        (source / "Api.java").write_text(
            "package api; public class Api extends dep.Base {\n"
            f" public static double {method}(double value) {{ return value; }}\n"
            " public static double degrees(int value) { return value; }\n"
            f" public static {shape} Shape {{}}\n"
            "}\n"
        )
        element = "Integer" if changed else "String"
        (source / "Generic.java").write_text(
            "package api; public class Generic {\n"
            f" public java.util.List<{element}> values() {{ return null; }}\n"
            "}\n"
        )
        classes = snapshot / "classes"
        run(
            "javac",
            "-cp",
            str(dependency),
            "-d",
            str(classes),
            str(source / "Api.java"),
            str(source / "Generic.java"),
        )
        run("jar", "cf", str(snapshot / "java/api.jar"), "-C", str(classes), ".")

    def test_layout_and_java_breakages_are_advisory(self):
        self.native(self.base)
        self.native(self.head, changed=True)
        self.java(self.base)
        self.java(self.head, changed=True)
        args = argparse.Namespace(
            base=self.base,
            head=self.head,
            output=self.output,
            base_revision="old",
            head_revision="new",
            abidiff=ABIDIFF,
            japicmp=Path(JAPICMP),
        )
        self.assertEqual(compatibility.compare(args), 0)
        report = json.loads((self.output / "results.json").read_text())
        self.assertEqual(report["native"][0]["status"], "changed")
        native = (self.output / report["native"][0]["report"]).read_text()
        self.assertIn("offset", native)
        self.assertIn("size", native)
        classes = {entry["class"]: entry for entry in report["java"]}
        self.assertTrue(classes["api.Api"]["binary"])
        self.assertTrue(classes["api.Api$Shape"]["binary"])
        self.assertFalse(classes["api.Generic"]["binary"])
        self.assertTrue(classes["api.Generic"]["source"])
        self.assertIn("METHOD_REMOVED", classes["api.Api"]["changes"])
        self.assertFalse(report["errors"])

    def test_unchanged_apis(self):
        self.native(self.base)
        self.native(self.head)
        self.java(self.base)
        self.java(self.head)
        result = compatibility.compare_native(
            self.base, self.head, self.output, ABIDIFF
        )
        self.assertEqual(result[0]["status"], "unchanged")
        self.assertEqual(
            compatibility.compare_java(
                self.base, self.head, self.output, Path(JAPICMP)
            ),
            [],
        )

    def test_missing_dwarf_is_an_error(self):
        self.native(self.base, debug=False)
        self.native(self.head, changed=True, debug=False)
        result = compatibility.compare_native(
            self.base, self.head, self.output, ABIDIFF
        )
        self.assertEqual(result[0]["status"], "error")

    def test_identical_artifact_does_not_need_abidiff(self):
        self.native(self.base, split_debug=True)
        shutil.copyfile(self.base / "native/api.zip", self.head / "native/api.zip")
        result = compatibility.compare_native(
            self.base, self.head, self.output, "/not-installed/abidiff"
        )
        self.assertEqual(result[0]["status"], "unchanged")

    def test_separate_debug_files_preserve_layout_checks(self):
        self.native(self.base, split_debug=True)
        self.native(self.head, changed=True, split_debug=True)
        result = compatibility.compare_native(
            self.base, self.head, self.output, ABIDIFF
        )
        self.assertEqual(result[0]["status"], "changed")
        self.assertIn("offset", (self.output / result[0]["report"]).read_text())

    def test_missing_java_dependency_is_an_error(self):
        self.java(self.base)
        self.java(self.head)
        (self.head / "classpath/support.jar").unlink()
        with self.assertRaises(ValueError):
            compatibility.compare_java(self.base, self.head, self.output, Path(JAPICMP))

    def test_removed_native_library_is_reported(self):
        self.native(self.base)
        self.native(self.head)
        (self.head / "native/api.zip").rename(self.head / "native/renamed.zip")
        results = compatibility.compare_native(
            self.base, self.head, self.output, ABIDIFF
        )
        self.assertEqual([r["status"] for r in results], ["removed", "added"])


if __name__ == "__main__":
    unittest.main()
