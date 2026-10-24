# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Published baseline selection, downloads, and reconstruction without builds."""

import json
import sys
import tempfile
import unittest
import urllib.error
import xml.etree.ElementTree as ET
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import compatibility_artifactory as artifactory
import compatibility_artifacts as artifacts

BASE = "a" * 40
RECENT = "b" * 40
HEAD = "c" * 40
SIDE = "d" * 40
VERSION = "2027.0.0-alpha-7-10-gaaaaaaa"
SOURCES = {
    "wpiutil/build.gradle": """
ext { nativeName = 'wpiutil' }
apply from: "${rootDir}/shared/jni/setupBuild.gradle"
dependencies {
    api libs.avaje.jsonb
    api(libs.quickbuf.runtime)
}
""",
    "wpinet/build.gradle": """
ext { nativeName = 'wpinet'; groupId = 'org.wpilib.net' }
apply from: "${rootDir}/shared/jni/setupBuild.gradle"
""",
    "simulation/halsim_gui/build.gradle": "pluginName = 'halsim_gui'",
    "shared/java/javacommon.gradle": "exclude(group: 'io.avaje', module: 'avaje-jsonb-inject-plugin')",
    "gradle/libs.versions.toml": """
[versions]
avaje = "3.14"
[libraries]
avaje-jsonb = { module = "io.avaje:avaje-jsonb", version.ref = "avaje" }
quickbuf-runtime = { module = "us.hebi.quickbuf:quickbuf-runtime", version = "1.4" }
""",
}


class ArtifactoryTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.addCleanup(artifactory.read_url.cache_clear)
        self.addCleanup(artifactory.storage.cache_clear)

    def test_exact_commit_and_tag_before_recent_versions_excluding_candidate_and_side_branch(
        self,
    ):
        versions = [
            VERSION,
            "2027.0.0-alpha-7-11-gbbbbbbb",
            "2027.0.0-alpha-7-12-gccccccc",
            "2027.0.0-alpha-7-13-gddddddd",
            "2027.0.0-alpha-7",
        ]
        xml = (
            "<metadata><versioning><versions>"
            + "".join(f"<version>{v}</version>" for v in versions)
            + "</versions></versioning></metadata>"
        )

        def git(repo, *args):
            if args[0] == "describe":
                return "v2027.0.0-alpha-7"
            if args[0] == "rev-list":
                return f"{HEAD}\n{RECENT}\n{BASE}"
            ref = args[-1]
            if ref.startswith("refs/remotes/"):
                return HEAD
            if ref.startswith("refs/tags/"):
                return BASE
            return {"a": BASE, "b": RECENT, "c": HEAD, "d": SIDE}[ref[0]]

        with (
            mock.patch.object(artifactory, "sources", return_value=SOURCES),
            mock.patch.object(artifactory, "read_url", return_value=xml.encode()),
            mock.patch.object(artifactory, "git", side_effect=git),
        ):
            result = artifactory.version_candidates(self.root, BASE, HEAD, "stable")
        self.assertEqual(result[0]["revision"], BASE)
        self.assertEqual(result[0]["repository"], "release")
        self.assertEqual({v["revision"] for v in result}, {BASE, RECENT})
        self.assertTrue(all(v["revision"] == BASE for v in result[:8]))
        self.assertEqual(result[-1]["revision"], RECENT)

    def test_stale_branch_ref_uses_requested_history_for_fallback(self):
        metadata = (
            "<metadata><versioning><versions>"
            "<version>2027.0.0-alpha-7-11-gbbbbbbb</version>"
            "</versions></versioning></metadata>"
        )

        def git(repo, *args):
            if args[0] == "describe":
                return "v2027.0.0-alpha-7"
            if args[0] == "rev-list":
                return f"{BASE}\n{RECENT}\n{SIDE}" if args[-1] == BASE else SIDE
            if args[-1].startswith("refs/remotes/"):
                return SIDE
            return RECENT

        with (
            mock.patch.object(artifactory, "sources", return_value=SOURCES),
            mock.patch.object(artifactory, "read_url", return_value=metadata.encode()),
            mock.patch.object(artifactory, "git", side_effect=git),
        ):
            versions = artifactory.version_candidates(self.root, BASE, HEAD, "main")
        self.assertTrue(versions)
        self.assertTrue(all(v["revision"] == RECENT for v in versions))

    def test_incomplete_exact_version_uses_recent_complete_version_per_platform(self):
        versions = [
            {"repository": "development", "version": VERSION, "revision": BASE},
            {
                "repository": "development",
                "version": "2027.0.0-alpha-7-11-gbbbbbbb",
                "revision": RECENT,
            },
        ]

        def inventory(repo, selected, platform):
            if selected["revision"] == BASE and platform != "linuxx86-64":
                raise ValueError("Missing debug archive")
            return {}

        with (
            mock.patch.object(artifactory, "version_candidates", return_value=versions),
            mock.patch.object(artifactory, "inventory", side_effect=inventory),
        ):
            result = artifactory.select(
                self.root, BASE, HEAD, "main", artifacts.TARGETS
            )
        self.assertEqual(result[0]["baseline_revision"], BASE)
        self.assertEqual(result[0]["baseline_warning"], "")
        for target in result[1:]:
            self.assertEqual(target["baseline_revision"], RECENT)
            self.assertIn(BASE, target["baseline_warning"])
            self.assertIn(RECENT, target["baseline_warning"])
            self.assertIn(versions[1]["version"], target["baseline_warning"])

    def test_unavailable_publications_fail_without_starting_a_build(self):
        with (
            mock.patch.object(artifactory, "version_candidates", return_value=[]),
            mock.patch.object(artifactory.subprocess, "run") as process,
        ):
            result = artifactory.select(
                self.root, BASE, HEAD, "main", artifacts.TARGETS
            )
        process.assert_not_called()
        self.assertTrue(all(t["baseline_error"] for t in result))

    def test_inventory_uses_baseline_publications_and_checks_both_configurations(self):
        def storage(repo, folder):
            artifact = folder.split("/")[-2]
            if artifact.endswith("-java"):
                files = [f"{artifact}-{VERSION}.jar"]
            else:
                files = [
                    f"{artifact}-{VERSION}-linuxx86-64{suffix}.zip"
                    for suffix in ("", "debug")
                ]
            return {"children": [{"uri": "/" + f, "folder": False} for f in files]}

        selection = {"repository": "development", "version": VERSION, "revision": BASE}
        with (
            mock.patch.object(artifactory, "sources", return_value=SOURCES),
            mock.patch.object(artifactory, "storage", side_effect=storage),
        ):
            inventory = artifactory.inventory(self.root, selection, "linuxx86-64")
        self.assertEqual(len(inventory["release"]["java"]), 2)
        self.assertEqual(len(inventory["debug"]["native"]), 3)
        self.assertIn(
            "simulation-halsim_gui-halsim_gui.zip",
            [v["name"] for v in inventory["debug"]["native"]],
        )
        self.assertTrue(
            any(
                "org/wpilib/wpinet/wpinet-java/" in v["path"]
                for v in inventory["release"]["java"]
            )
        )
        with (
            mock.patch.object(artifactory, "sources", return_value=SOURCES),
            mock.patch.object(artifactory, "storage", return_value={"children": []}),
            self.assertRaisesRegex(ValueError, "Missing published artifact"),
        ):
            artifactory.inventory(self.root, selection, "linuxx86-64")

    def test_classpath_uses_selected_revision_catalog_and_exclusions(self):
        dependencies, exclusions = artifactory.external_dependencies(
            SOURCES, artifactory.publications(SOURCES)
        )
        self.assertEqual(
            dependencies,
            [
                ("io.avaje", "avaje-jsonb", "3.14"),
                ("us.hebi.quickbuf", "quickbuf-runtime", "1.4"),
            ],
        )
        self.assertEqual(exclusions, [("io.avaje", "avaje-jsonb-inject-plugin")])
        pom = artifactory.dependency_pom(dependencies, exclusions, ["development"])
        xml = ET.fromstring(pom)
        ns = {"m": "http://maven.apache.org/POM/4.0.0"}
        self.assertEqual(len(xml.findall("m:dependencies/m:dependency", ns)), 2)
        self.assertIsNone(xml.find("m:build", ns))
        self.assertIn("/api/download/development", pom)

    def test_download_records_checksums_and_rejects_corrupt_bytes(self):
        entries = {
            "java": [
                {
                    "file": "api.jar",
                    "name": "wpiutil.jar",
                    "path": "org/wpilib/wpiutil/api.jar",
                }
            ],
            "native": [
                {
                    "file": "api.zip",
                    "name": "wpiutil-wpiutil-cpp.zip",
                    "path": "org/wpilib/wpiutil/api.zip",
                }
            ],
        }
        inventory = {c: entries for c in ("release", "debug")}
        selection = {"repository": "development", "version": VERSION, "revision": BASE}
        expected = "e" * 64

        def fetch(request, destination):
            (destination / request["path"]).write_bytes(b"artifact")
            return {
                "status": "downloaded",
                "sha256": expected,
                "url": request["urls"][0],
                "bytes": 8,
            }

        with (
            mock.patch.object(artifactory, "inventory", return_value=inventory),
            mock.patch.object(artifactory, "sources", return_value=SOURCES),
            mock.patch.object(
                artifactory, "storage", return_value={"checksums": {"sha256": expected}}
            ),
            mock.patch.object(
                artifactory.compatibility_vendors,
                "download_artifact",
                side_effect=fetch,
            ) as downloads,
        ):
            artifactory.download(
                self.root, selection, "linuxx86-64", self.root / "products"
            )
            manifest = json.loads(
                (self.root / "products/compatibility/manifest.json").read_text()
            )
            self.assertEqual(manifest["version"], VERSION)
            self.assertEqual(manifest["source"], "artifactory")
            self.assertEqual(
                downloads.call_count, 2
            )  # Java jars are shared by both configs.
            self.assertTrue((self.root / "products/compatibility/complete").exists())
            downloads.side_effect = lambda request, dest: {
                **fetch(request, dest),
                "sha256": "f" * 64,
            }
            with self.assertRaisesRegex(ValueError, "SHA-256 mismatch"):
                artifactory.download(
                    self.root, selection, "linuxx86-64", self.root / "corrupt"
                )
            self.assertFalse((self.root / "corrupt/compatibility/complete").exists())

    def test_storage_only_treats_404_as_absence(self):
        for code in (404, 403, 500):
            artifactory.storage.cache_clear()
            with mock.patch.object(
                artifactory,
                "read_url",
                side_effect=urllib.error.HTTPError(
                    "https://example.invalid", code, "error", {}, None
                ),
            ):
                if code == 404:
                    self.assertIsNone(artifactory.storage("development", "missing"))
                else:
                    with self.assertRaises(urllib.error.HTTPError):
                        artifactory.storage("development", "missing")


if __name__ == "__main__":
    unittest.main()
