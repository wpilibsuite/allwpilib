# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Artifact provenance, supplemental debug data, and grouped report coverage."""

import argparse
import json
import subprocess
import sys
import tempfile
import unittest
import zipfile
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import compatibility_artifacts as artifacts

REVISION = "a" * 40


class ArtifactTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)

    def products(
        self,
        platform="linuxx86-64",
        configuration="release",
        name="products",
        revision=REVISION,
    ):
        products = self.root / name
        extras = products / "compatibility"
        extras.mkdir(parents=True)
        target = {
            "java": [{"file": "published.jar", "name": "api.jar"}],
            "native": [{"file": "published.zip", "name": "api.zip"}],
        }
        manifest = {
            "schema": 1,
            "revision": revision,
            "platform": platform,
            "targets": {configuration: target},
        }
        (extras / "manifest.json").write_text(json.dumps(manifest))
        (products / "published.jar").write_bytes(b"published Java bytecode")
        with zipfile.ZipFile(products / "published.zip", "w") as archive:
            archive.writestr(
                "osx/universal/shared/libapi.dylib"
                if platform == "osxuniversal"
                else "linux/x86-64/shared/libapi.so",
                b"published native binary",
            )
        (extras / "classpath").mkdir()
        (extras / "classpath/support.jar").write_bytes(b"external dependency")
        return products

    def test_selects_only_exact_commit_push_artifacts_per_platform(self):
        def run(number, **changes):
            return {
                "id": number,
                "head_sha": REVISION,
                "event": "push",
                "status": "completed",
                **changes,
            }

        runs = [
            run(1, event="pull_request"),
            run(2, head_sha="b" * 40),
            run(3, status="in_progress"),
            run(4),
            run(5),
        ]
        calls = []

        def available(run):
            number = run["id"]
            calls.append(number)
            return [
                {"name": name, "expired": number == 4 and name == "Linux"}
                for name in ("Linux", "Win64Release", "Win64Debug")
                if not (number == 3 and name == "Win64Debug")
            ]

        plan = artifacts.artifact_plan(REVISION, runs, available)
        self.assertEqual(calls, [3, 4, 5])
        by_platform = {t["platform"]: t for t in plan}
        self.assertEqual(by_platform["linuxx86-64"]["baseline_run"], "3")
        self.assertEqual(by_platform["windowsx86-64"]["baseline_run"], "4")
        self.assertEqual(
            by_platform["windowsx86-64"]["pattern"], "{Win64Debug,Win64Release}"
        )
        self.assertEqual(by_platform["osxuniversal"]["baseline_run"], "")

    def test_recent_artifact_fallback_preserves_exact_matches_and_excludes_candidate(
        self,
    ):
        def run(number, revision, **changes):
            return {
                "id": number,
                "head_sha": revision,
                "head_branch": "main",
                "event": "push",
                **changes,
            }

        candidate = "d" * 40
        recent = "b" * 40
        older = "c" * 40
        exact = [run(20, REVISION)]
        fallback = [
            run(10, candidate),
            run(9, recent, event="pull_request"),
            run(8, recent, head_branch="stable"),
            run(7, recent),
            run(6, older),
        ]
        visited = []

        def available(run):
            visited.append(run["id"])
            names = (
                ["Linux"]
                if run["id"] == 20
                else [a for t in artifacts.TARGETS for a in t["artifacts"]]
            )
            return [
                {"name": name, "expired": run["id"] == 7 and name == "macOS"}
                for name in names
            ]

        targets = artifacts.artifact_plan(
            REVISION,
            exact,
            available,
            fallback_runs=lambda: fallback,
            branch="main",
            candidate=candidate,
        )
        by_platform = {t["platform"]: t for t in targets}
        self.assertEqual(visited, [20, 7, 6])
        self.assertEqual(by_platform["linuxx86-64"]["baseline_revision"], REVISION)
        self.assertEqual(by_platform["linuxx86-64"]["baseline_warning"], "")
        self.assertEqual(by_platform["windowsx86-64"]["baseline_revision"], recent)
        self.assertEqual(by_platform["osxuniversal"]["baseline_revision"], older)
        self.assertIn(REVISION, by_platform["osxuniversal"]["baseline_warning"])
        self.assertIn(older, by_platform["osxuniversal"]["baseline_warning"])

    def test_planner_skips_artifacts_with_failed_supplement_and_reports_missing_inputs(
        self,
    ):
        args = argparse.Namespace(
            repo=self.root,
            revision=REVISION,
            candidate="d" * 40,
            branch="main",
            repository="wpilibsuite/allwpilib",
            output=self.root / "outputs",
        )

        def api(repository, endpoint, key, **query):
            if key == "workflow_runs":
                return [
                    {
                        "id": 1,
                        "head_sha": REVISION,
                        "head_branch": "main",
                        "event": "push",
                    }
                ]
            if key == "jobs":
                return [
                    {
                        "name": "Build - " + name,
                        "steps": [
                            {
                                "name": "Add compatibility data to build artifacts",
                                "conclusion": "failure",
                            }
                        ],
                    }
                    for target in artifacts.TARGETS
                    for name in target["artifacts"]
                ]
            return [
                {"name": name, "expired": False}
                for target in artifacts.TARGETS
                for name in target["artifacts"]
            ]

        with (
            mock.patch.object(artifacts, "api_pages", side_effect=api),
            mock.patch.object(
                artifacts.subprocess,
                "run",
                return_value=subprocess.CompletedProcess([], 0),
            ),
        ):
            artifacts.plan(args)
        lines = args.output.read_text().splitlines()
        self.assertEqual(len(lines), 1)
        targets = json.loads(lines[0].removeprefix("matrix="))["include"]
        self.assertEqual(len(targets), 6)
        for target in targets:
            self.assertEqual(target["baseline_run"], "")
            self.assertIn("No usable Gradle artifacts", target["baseline_error"])

    def report_args(self, platform="linuxx86-64"):
        return argparse.Namespace(
            output=self.root / "report",
            repo=self.root,
            inventory=self.root,
            candidate_products=self.root / "candidate",
            baseline_products=self.root / "base",
            platform=platform,
            base_revision=REVISION,
            base_label="baseline",
            head_revision="b" * 40,
            japicmp=self.root / "japicmp.jar",
        )

    def test_missing_artifacts_fail_before_downloads_or_analysis(self):
        args = self.report_args()
        with (
            mock.patch.object(artifacts.subprocess, "run") as processes,
            mock.patch.object(artifacts.compatibility, "compare") as compare,
        ):
            self.assertEqual(artifacts.report(args), 1)
        processes.assert_not_called()
        compare.assert_not_called()
        result = json.loads((args.output / "results.json").read_text())
        self.assertEqual(len(result["errors"]), 4)
        self.assertIn("compatibility/manifest.json", result["errors"][0])
        self.assertIn(
            "No rebuild was attempted", (args.output / "summary.md").read_text()
        )

    def test_preflight_checks_all_configs_and_records_baseline_deviation(self):
        args = self.report_args()
        args.requested_base = "c" * 40
        args.baseline_warning = f"Requested {args.requested_base}; using {REVISION}."
        for side, revision in (("base", REVISION), ("candidate", args.head_revision)):
            for configuration in ("release", "debug"):
                products = self.products(
                    configuration=configuration,
                    name=f"{side}/{configuration}",
                    revision=revision,
                )
                artifacts.prepare(products)
        self.assertEqual(artifacts.validate(args), 0)
        (self.root / "candidate/debug/published.zip").unlink()
        self.assertEqual(artifacts.validate(args), 1)
        result = json.loads((args.output / "results.json").read_text())
        self.assertEqual(result["requested_base"], args.requested_base)
        self.assertEqual(result["baseline_warning"], args.baseline_warning)
        self.assertEqual(len(result["errors"]), 1)
        self.assertIn("Candidate debug", result["errors"][0])
        self.assertIn("Baseline warning", (args.output / "summary.md").read_text())

    def test_existing_artifacts_keep_binaries_classpath_and_sidecars(self):
        products = self.products()
        sidecar = products / "compatibility/release/native-debug/api/libapi.so.debug"
        sidecar.parent.mkdir(parents=True)
        sidecar.write_bytes(b"DWARF")
        original = (products / "published.zip").read_bytes()
        artifacts.prepare(products)
        dest = self.root / "snapshot"
        artifacts.materialize(products, dest, REVISION, "linuxx86-64", "release")
        self.assertEqual((dest / "native/api.zip").read_bytes(), original)
        self.assertEqual(
            (dest / "java/api.jar").read_bytes(), b"published Java bytecode"
        )
        self.assertEqual(
            (dest / "classpath/support.jar").read_bytes(), b"external dependency"
        )
        self.assertEqual(
            (dest / "native-debug/api/libapi.so.debug").read_bytes(), b"DWARF"
        )
        self.assertTrue((dest / "complete").is_file())
        inputs = dict(
            artifacts.compatibility.native_files(dest, dest / "native/api.zip")
        )
        self.assertEqual(inputs["libapi.so.debug"], b"DWARF")

    def test_windows_artifacts_are_selected_by_configuration(self):
        for configuration in ("release", "debug"):
            products = self.products(
                "windowsx86-64", configuration, f"products/Win64{configuration}"
            )
            artifacts.prepare(products)
            (products / "published.jar").write_text(configuration)
        for configuration in ("release", "debug"):
            dest = self.root / configuration
            artifacts.materialize(
                self.root / "products", dest, REVISION, "windowsx86-64", configuration
            )
            self.assertEqual((dest / "java/api.jar").read_text(), configuration)

    def test_wrong_revision_unfinished_or_ambiguous_artifacts_are_rejected(self):
        products = self.products()
        dest = self.root / "snapshot"
        with self.assertRaisesRegex(ValueError, "incomplete"):
            artifacts.materialize(products, dest, REVISION, "linuxx86-64", "release")
        artifacts.prepare(products)
        with self.assertRaisesRegex(ValueError, "does not match"):
            artifacts.materialize(products, dest, "b" * 40, "linuxx86-64", "release")
        extra = self.products(name="products/duplicate")
        artifacts.prepare(extra)
        with self.assertRaisesRegex(ValueError, "found 2"):
            artifacts.materialize(products, dest, REVISION, "linuxx86-64", "release")
        self.assertFalse((dest / "complete").exists())

    def test_unsafe_paths_and_missing_published_files_are_rejected(self):
        products = self.products()
        manifest_path = products / "compatibility/manifest.json"
        manifest = json.loads(manifest_path.read_text())
        for bad in ("../outside.jar", "C:\\outside.jar", "/outside.jar"):
            manifest["targets"]["release"]["java"][0]["file"] = bad
            manifest_path.write_text(json.dumps(manifest))
            with self.assertRaisesRegex(ValueError, "filename"):
                artifacts.prepare(products)
        manifest["targets"]["release"]["java"][0]["file"] = "missing.jar"
        manifest_path.write_text(json.dumps(manifest))
        with self.assertRaisesRegex(ValueError, "Missing published"):
            artifacts.prepare(products)
        self.assertFalse((products / "compatibility/complete").exists())

    def test_macos_debug_link_does_not_repackage_binaries_and_failure_is_incomplete(
        self,
    ):
        products = self.products("osxuniversal")
        original = (products / "published.zip").read_bytes()

        def dsymutil(command, **kwargs):
            self.assertEqual(Path(command[-1]).read_bytes(), b"published native binary")
            Path(command[3]).write_bytes(b"linked DWARF")

        with (
            mock.patch.object(artifacts, "llvm_tool", return_value="dsymutil"),
            mock.patch.object(artifacts.subprocess, "run", side_effect=dsymutil),
        ):
            artifacts.prepare(products)
        self.assertEqual((products / "published.zip").read_bytes(), original)
        self.assertEqual(
            (
                products / "compatibility/release/native-debug/api/libapi.dylib.debug"
            ).read_bytes(),
            b"linked DWARF",
        )
        with (
            mock.patch.object(artifacts, "llvm_tool", return_value="dsymutil"),
            mock.patch.object(
                artifacts.subprocess,
                "run",
                side_effect=subprocess.CalledProcessError(1, "dsymutil"),
            ),
            self.assertRaises(subprocess.CalledProcessError),
        ):
            artifacts.prepare(products)
        self.assertFalse((products / "compatibility/complete").exists())

    def test_grouped_report_keeps_all_configurations_and_architectures_after_error(
        self,
    ):
        args = self.report_args("osxuniversal")
        args.requested_base = "c" * 40
        args.baseline_warning = f"Requested {args.requested_base}; using {REVISION}."
        for side, revision in (("base", REVISION), ("candidate", args.head_revision)):
            for configuration in ("release", "debug"):
                products = self.products(
                    "osxuniversal", configuration, f"{side}/{configuration}", revision
                )
                (products / "compatibility/complete").touch()
        calls = []

        def compare(options):
            self.assertEqual(options.requested_base, args.requested_base)
            self.assertEqual(options.baseline_warning, args.baseline_warning)
            calls.append((options.output.parent.name, options.architecture))
            (options.output / "summary.md").write_text(
                "INCOMPLETE" if len(calls) == 1 else "Complete"
            )
            return int(len(calls) == 1)

        with (
            mock.patch.object(artifacts, "materialize"),
            mock.patch.object(
                artifacts.subprocess,
                "run",
                return_value=subprocess.CompletedProcess([], 0),
            ) as downloads,
            mock.patch.object(artifacts.compatibility, "compare", side_effect=compare),
        ):
            self.assertEqual(artifacts.report(args), 1)
        self.assertEqual(
            calls,
            [
                ("release", "x86_64"),
                ("release", "arm64"),
                ("debug", "x86_64"),
                ("debug", "arm64"),
            ],
        )
        self.assertEqual(downloads.call_count, 2)
        for call in downloads.call_args_list:
            command = call.args[0]
            self.assertEqual(command[command.index("--baseline") + 1], REVISION)
        self.assertIn("INCOMPLETE", (args.output / "summary.md").read_text())
        self.assertIn(
            "osxuniversal debug arm64", (args.output / "summary.md").read_text()
        )


if __name__ == "__main__":
    unittest.main()
