# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Vendor downloads and static consumer evidence, including compiled regressions."""

import argparse
import io
import json
import os
import shutil
import sys
import tempfile
import unittest
import urllib.error
import zipfile
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import compatibility
import compatibility_java as java
import compatibility_native as native
import compatibility_vendors as vendors
import test_compatibility as fixtures
from test_compatibility import ABIDIFF, JAPICMP, run


class DownloadTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)

    def test_series_and_missing_inventory(self):
        for tag, expected in (
            ("v2027.0.0-alpha-7", "2027_alpha7"),
            ("v2026.2.1", "2026"),
            ("v2027.1.1-beta-2", "2027beta"),
        ):
            self.assertEqual(vendors.vendor_series(tag), expected)
        for tag in ("main", "v2027.0.0-other-1"):
            with self.assertRaises(ValueError):
                vendors.vendor_series(tag)
        args = argparse.Namespace(
            repo=self.root,
            inventory=self.root,
            baseline="old",
            series=None,
            output=self.root / "download",
        )
        with patch(
            "compatibility_vendors.subprocess.check_output",
            side_effect=["v2027.0.0-alpha-7\n", "deadbeef\n"],
        ):
            self.assertEqual(vendors.download(args), 1)
        index = json.loads((args.output / "index.json").read_text())
        self.assertIn("No vendor manifests", index["errors"][0])

    def test_classifier_selection_and_deduplication(self):
        dep = {"groupId": "org.example", "artifactId": "library", "version": "1.0"}
        manifest = {
            "file": "vendor.json",
            "mavenUrls": ["https://frcmaven.wpi.edu/artifactory/vendor/"],
            "javaDependencies": [dep],
            "cppDependencies": [
                dict(dep, binaryPlatforms=["linuxx86-64"], sharedLibrary=False)
            ],
            "jniDependencies": [
                dict(dep, validPlatforms=["linuxx86-64"], isJar=True),
                dict(dep, validPlatforms=["linuxarm64"], isJar=False),
            ],
        }
        requests, skipped = vendors.artifact_requests(
            [manifest, dict(manifest, file="another.json")]
        )
        self.assertEqual(len(requests), 3)
        self.assertEqual(len(skipped), 2)
        self.assertTrue(
            any(r["path"].endswith("-linuxx86-64static.zip") for r in requests)
        )
        self.assertTrue(any(r["path"].endswith("-linuxx86-64.jar") for r in requests))
        self.assertTrue(all(len(r["manifests"]) == 2 for r in requests))
        self.assertIn("/artifactory/api/download/vendor/", requests[0]["urls"][0])
        dep["version"] = "../../escape"
        with self.assertRaises(ValueError):
            vendors.artifact_requests([manifest])

    def test_download_fallback_rejects_html_and_records_hash(self):
        data = io.BytesIO()
        with zipfile.ZipFile(data, "w") as archive:
            archive.writestr("example.txt", "content")
        urls = ["https://first.example/a.jar", "https://second.example/a.jar"]
        html = io.BytesIO(b"<html>login</html>")
        html.url = urls[0]
        valid = io.BytesIO(data.getvalue())
        valid.url = urls[1]
        with patch(
            "compatibility_vendors.urllib.request.urlopen", side_effect=[html, valid]
        ):
            result = vendors.download_artifact(
                {"path": "a.jar", "urls": urls}, self.root
            )
        self.assertEqual(result["status"], "downloaded")
        self.assertEqual(result["url"], urls[1])
        self.assertEqual(result["sha256"], vendors.digest(self.root / "a.jar"))
        self.assertEqual(len(result["attempts"]), 1)
        self.assertFalse((self.root / "a.jar.part").exists())

    def test_unavailable_artifact_and_checksum_mismatch(self):
        with patch(
            "compatibility_vendors.urllib.request.urlopen",
            side_effect=urllib.error.URLError("unavailable"),
        ):
            result = vendors.download_artifact(
                {"path": "a.jar", "urls": ["https://example.com/a.jar"]}, self.root
            )
        self.assertEqual(result["status"], "unavailable")
        (self.root / "artifacts").mkdir()
        (self.root / "artifacts/a.jar").write_bytes(b"changed")
        (self.root / "index.json").write_text(
            json.dumps(
                {
                    "artifacts": [
                        {"status": "downloaded", "path": "a.jar", "sha256": "invalid"}
                    ]
                }
            )
        )
        output = self.root / "report"
        output.mkdir()
        with self.assertRaisesRegex(ValueError, "checksum mismatch"):
            vendors.load_inventory(self.root, output)

    def test_failed_selection_still_produces_summary(self):
        (self.root / "index.json").write_text(
            json.dumps({"artifacts": [], "skipped": [], "errors": ["No inventory"]})
        )
        args = argparse.Namespace(
            base=self.root / "base",
            head=self.root / "head",
            output=self.root / "report",
            base_revision="old",
            head_revision="new",
            vendors=self.root,
        )
        self.assertEqual(compatibility.compare(args), 1)
        self.assertIn("INCOMPLETE", (args.output / "summary.md").read_text())


@unittest.skipUnless(
    JAPICMP and shutil.which(ABIDIFF) and shutil.which("g++") and shutil.which("javac"),
    "Install comparison tools to run compiled vendor fixtures",
)
class VendorToolTests(unittest.TestCase):
    setUp = fixtures.ToolTests.setUp
    native = fixtures.ToolTests.native
    java = fixtures.ToolTests.java

    def test_java_exact_member_inherited_call_and_generic_change(self):
        self.java(self.base)
        self.java(self.head, changed=True)
        compatibility.compare_java(self.base, self.head, self.output, Path(JAPICMP))
        source = self.root / "Client.java"
        source.write_text(
            "package vendor; class Sub extends api.Api {} "
            "public class Client { public double call() { return Sub.degrees(1.0); } "
            "public String generic(api.Generic g) { return g.values().get(0); } } "
            "class Unrelated { api.Api onlyAType; double call() { return api.Api.degrees(1); } } "
            "class Overridden extends api.Api { public static double degrees(double v) { return v; } "
            "double call() { return degrees(1.0); } }"
        )
        classes = self.root / "vendor-classes"
        run(
            "javac",
            "-cp",
            os.pathsep.join(
                [
                    str(self.base / "java/api.jar"),
                    str(self.base / "classpath/support.jar"),
                ]
            ),
            "-d",
            str(classes),
            str(source),
        )
        jar = self.root / "vendor.jar"
        dependency = self.root / "sub.jar"
        with zipfile.ZipFile(dependency, "w") as archive:
            archive.write(classes / "vendor/Sub.class", "vendor/Sub.class")
        (classes / "vendor/Sub.class").unlink()
        run("jar", "cf", str(jar), "-C", str(classes), ".")
        result = java.trace_java(
            [
                ({"path": "vendor.jar", "manifests": ["vendor.json"]}, jar),
                ({"path": "sub.jar", "manifests": ["vendor.json"]}, dependency),
            ],
            self.base,
            self.output / "java.xml",
        )
        self.assertFalse(result["errors"])
        self.assertEqual(result["scanned"], 2)
        self.assertEqual([f["class"] for f in result["findings"]], ["vendor.Client"])
        refs = {r["target"]: r for r in result["findings"][0]["references"]}
        self.assertIn("api.Api.degrees(D)D", refs)
        self.assertEqual(refs["api.Api.degrees(D)D"]["reference_owner"], "vendor.Sub")
        self.assertIn("api.Generic.values()Ljava/util/List;", refs)
        # Another manifest uses a different Sub version that declares its own method.
        source = self.root / "Sub.java"
        source.write_text(
            "package vendor; class Sub { static double degrees(double v) { return v; } }"
        )
        run("javac", "-d", str(self.root / "other-classes"), str(source))
        other = self.root / "other.jar"
        run("jar", "cf", str(other), "-C", str(self.root / "other-classes"), ".")
        result = java.trace_java(
            [
                (
                    {"path": "vendor.jar", "manifests": ["vendor.json", "other.json"]},
                    jar,
                ),
                ({"path": "sub.jar", "manifests": ["vendor.json"]}, dependency),
                ({"path": "other.jar", "manifests": ["other.json"]}, other),
            ],
            self.base,
            self.output / "java.xml",
        )
        degree_refs = [
            f
            for f in result["findings"]
            if any(r["target"] == "api.Api.degrees(D)D" for r in f["references"])
        ]
        self.assertEqual(len(degree_refs), 1)
        self.assertEqual(degree_refs[0]["manifests"], ["vendor.json"])

    def test_java_class_to_interface(self):
        self.java(self.base)
        self.java(self.head, changed=True)
        compatibility.compare_java(self.base, self.head, self.output, Path(JAPICMP))
        class_changes, members = java.changed_definitions(self.output / "java.xml")
        self.assertIn("api/Api$Shape", class_changes)
        self.assertIn("CLASS_TYPE_CHANGED", class_changes["api/Api$Shape"])
        caller = {"types": set(), "parents": ["vendor/Sub"], "refs": set()}
        hierarchy = {"vendor/Sub": {"parents": ["api/Api$Shape"]}}
        result = java.class_references(caller, hierarchy, class_changes, members)
        self.assertEqual(result[0]["target"], "api.Api$Shape")

    def test_java_field_descriptor(self):
        source = self.root / "Api.java"
        for snapshot, value_type in ((self.base, "String"), (self.head, "Integer")):
            source.write_text(
                f"package api; public class Api {{ public {value_type} value; }}"
            )
            run("javac", "-d", str(snapshot / "classes"), str(source))
            run(
                "jar",
                "cf",
                str(snapshot / "java/api.jar"),
                "-C",
                str(snapshot / "classes"),
                ".",
            )
        compatibility.compare_java(self.base, self.head, self.output, Path(JAPICMP))
        _, members = java.changed_definitions(self.output / "java.xml")
        self.assertIn(("field", "api/Api", "value", "Ljava/lang/String;"), members)

    def test_static_and_shared_native_layout_and_removed_symbol(self):
        self.native(self.base)
        self.native(self.head, changed=True)
        vendor_src = self.root / "vendor.cpp"
        vendor_src.write_text(
            "struct Layout; struct Callback; int consume(Layout*, Callback*); extern int removed(); "
            "int vendor(Layout* p, Callback* cb) { return consume(p, cb) + removed(); }"
        )
        run("g++", "-fPIC", "-c", str(vendor_src), "-o", str(self.root / "vendor.o"))
        run(
            "g++",
            "-shared",
            str(self.root / "vendor.o"),
            "-o",
            str(self.root / "vendor.so"),
        )
        run("ar", "rcs", str(self.root / "vendor.a"), str(self.root / "vendor.o"))
        removed_src = self.root / "removed.cpp"
        removed_src.write_text("int removed() { return 1; }")
        run(
            "g++",
            "-shared",
            "-fPIC",
            "-g",
            str(removed_src),
            "-o",
            str(self.root / "removed.so"),
        )
        archives = []
        for extension in ("a", "so"):
            path = self.root / ("vendor-" + extension + ".zip")
            with zipfile.ZipFile(path, "w") as archive:
                archive.write(
                    self.root / ("vendor." + extension), "../../vendor." + extension
                )
            archives.append(({"path": path.name, "manifests": ["vendor.json"]}, path))
        old = {
            "api/libapi.so": self.base / "libapi.so",
            "removed.so": self.root / "removed.so",
        }
        new = {"api/libapi.so": self.head / "libapi.so"}
        comparison = compatibility.compare_native_libraries(
            old, new, self.output, ABIDIFF
        )
        result = native.trace_native(
            archives, old, new, comparison, self.output, self.root
        )
        self.assertFalse(result["errors"])
        self.assertEqual(result["scanned"], 2)
        self.assertEqual(len(result["findings"]), 2)
        for finding in result["findings"]:
            refs = finding["references"]
            self.assertTrue(any(r.get("symbol") == "_Z7removedv" for r in refs))
            self.assertTrue(
                any(
                    m["kind"] == "changed interface reference"
                    for r in refs
                    for m in r.get("matches", [])
                )
            )
        # A symbol moved to another WPILib library is not a removed export.
        new["moved.so"] = old["removed.so"]
        result = native.trace_native(
            archives, old, new, comparison, self.output, self.root
        )
        self.assertFalse(
            any(
                r.get("kind") == "removed symbol"
                for f in result["findings"]
                for r in f["references"]
            )
        )

    def test_corrupt_vendor_is_incomplete(self):
        bad = self.root / "bad.jar"
        with zipfile.ZipFile(bad, "w") as archive:
            archive.writestr("Client.class", b"not a class")
        (self.output / "java.xml").write_text("<japicmp><classes/></japicmp>")
        artifact = {"path": bad.name, "manifests": ["vendor.json"]}
        result = java.trace_java([(artifact, bad)], self.base, self.output / "java.xml")
        self.assertEqual(result["scanned"], 0)
        self.assertTrue(result["errors"])
        result = native.trace_native(
            [(artifact, bad)], {}, {}, [], self.output, self.root
        )
        self.assertEqual(result["scanned"], 0)
        self.assertTrue(result["errors"])


if __name__ == "__main__":
    unittest.main()
