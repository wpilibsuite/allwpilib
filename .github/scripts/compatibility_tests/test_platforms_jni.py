# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Cross-compiled desktop ABI fixtures and private Java/JNI contract regressions."""

import argparse
import importlib.util
import json
import shutil
import sys
import tempfile
import unittest
import zipfile
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import compatibility
import compatibility_jni as jni
import compatibility_native as native
import compatibility_vendors as vendors
from compatibility_binary import llvm_tool, symbols
from test_compatibility import run


def available(name):
    try:
        return llvm_tool(name)
    except ValueError:
        return None


class PlatformTests(unittest.TestCase):
    def test_target_classifiers_include_simulation_jni_and_debug(self):
        dep = {
            "groupId": "org.vendor",
            "artifactId": "plugin",
            "version": "1",
            "validPlatforms": ["windowsarm64", "linuxsystemcore"],
            "isJar": False,
            "simMode": "swsim",
        }
        manifest = {
            "file": "vendor.json",
            "jniDependencies": [dep],
            "mavenUrls": ["https://example.com"],
        }
        requests, skipped = vendors.artifact_requests(
            [manifest], "windowsarm64", "debug"
        )
        self.assertFalse(skipped)
        self.assertTrue(requests[0]["path"].endswith("-windowsarm64debug.zip"))
        self.assertEqual(requests[0]["kinds"], ["jni"])
        requests, _ = vendors.artifact_requests([manifest], "linuxsystemcore")
        self.assertTrue(requests[0]["path"].endswith("-linuxsystemcore.zip"))
        requests, skipped = vendors.artifact_requests([manifest], "osxuniversal")
        self.assertFalse(requests)
        self.assertTrue(skipped)

    def test_mismatched_targets_do_not_compare_native_abis(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for side, target in [("base", "windowsx86-64"), ("head", "windowsarm64")]:
                (root / side).mkdir()
                (root / side / "complete").touch()
                (root / side / "target.json").write_text(
                    json.dumps({"platform": target, "configuration": "release"})
                )
            args = argparse.Namespace(
                base=root / "base",
                head=root / "head",
                output=root / "report",
                base_revision="old",
                head_revision="new",
                japicmp=None,
            )
            with (
                patch("compatibility.compare_native") as compare_native,
                patch("compatibility.compare_java", return_value=[]),
            ):
                self.assertEqual(compatibility.compare(args), 1)
                compare_native.assert_not_called()
            self.assertIn("metadata differ", (args.output / "summary.md").read_text())

    def test_jni_escaping_including_overloads(self):
        self.assertEqual(jni.escape("p/Foo_bar$Inner"), "p_Foo_1bar_00024Inner")
        self.assertEqual(jni.escape("[Ljava/lang/String;I"), "_3Ljava_lang_String_2I")
        self.assertEqual(jni.escape("\U0001f600"), "_0d83d_0de00")


@unittest.skipUnless(
    importlib.util.find_spec("lldb")
    and available("clang")
    and available("lld-link")
    and available("ld64.lld"),
    "Install LLVM/Clang/LLDB to run Windows and macOS fixtures",
)
class DesktopTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)

    def build(self, folder, platform, changed=False, fields=None, extra=""):
        folder.mkdir(parents=True, exist_ok=True)
        source = folder / "api.cpp"
        source.write_text(
            "struct Layout { "
            + (fields or ("int b; int a;" if changed else "int a; int b;"))
            + " };\n"
            "#ifdef _WIN32\n__declspec(dllexport)\n#endif\n"
            "int consume(Layout* p) { return p->a; }\n" + extra
        )
        if platform.startswith("windows"):
            triple = (
                "aarch64-pc-windows-msvc"
                if platform.endswith("arm64")
                else "x86_64-pc-windows-msvc"
            )
            run(
                llvm_tool("clang"),
                "--target=" + triple,
                "-g",
                "-fstandalone-debug",
                "-gcodeview",
                "-c",
                "-o",
                str(folder / "api.obj"),
                str(source),
            )
            library = folder / "api.dll"
            run(
                llvm_tool("lld-link"),
                "/dll",
                "/noentry",
                "/nodefaultlib",
                "/debug",
                "/pdb:" + str(folder / "api.pdb"),
                "/out:" + str(library),
                str(folder / "api.obj"),
            )
        else:
            architecture = platform
            run(
                llvm_tool("clang"),
                "--target=" + architecture + "-apple-macos13",
                "-g",
                "-fstandalone-debug",
                "-c",
                "-o",
                str(folder / "api.o"),
                str(source),
            )
            library = folder / "api.dylib"
            run(
                llvm_tool("ld64.lld"),
                "-dylib",
                "-arch",
                architecture,
                "-platform_version",
                "macos",
                "13.0",
                "13.0",
                "-o",
                str(library),
                str(folder / "api.o"),
            )
            run(
                llvm_tool("dsymutil"),
                "--flat",
                "-o",
                str(library) + ".debug",
                str(library),
            )
        return library

    def test_same_size_layout_changes_on_all_desktop_architectures(self):
        for platform in ("windowsx86-64", "windowsarm64", "x86_64", "arm64"):
            with self.subTest(platform=platform):
                old = self.build(self.root / platform / "old", platform)
                new = self.build(self.root / platform / "new", platform, changed=True)
                log = self.root / (platform + ".txt")
                self.assertEqual(
                    compatibility.compare_debug_types(old, old, log), "unchanged"
                )
                self.assertEqual(
                    compatibility.compare_debug_types(old, new, log), "changed"
                )
                self.assertIn("'class Layout' changed", log.read_text())
                self.assertEqual(
                    symbols(old, dynamic=True)[0], symbols(new, dynamic=True)[0]
                )

    def test_c_linkage_signature_and_data_changes(self):
        for platform in ("windowsx86-64", "x86_64"):
            with self.subTest(platform=platform):
                libs = []
                for label, value_type in [("old", "int"), ("new", "double")]:
                    extra = (
                        '#ifdef _WIN32\n#define API __declspec(dllexport)\nextern "C" int _fltused = 0;\n#else\n#define API\n#endif\nextern "C" { API '
                        + value_type
                        + " function() {return 0;} API "
                        + value_type
                        + " variable = 0; }\n"
                    )
                    libs.append(
                        self.build(self.root / platform / label, platform, extra=extra)
                    )
                log = self.root / (platform + ".txt")
                self.assertEqual(
                    compatibility.compare_debug_types(*libs, log), "changed"
                )
                self.assertIn("Function signature changed: function", log.read_text())
                self.assertIn("Variable type changed: variable", log.read_text())

    def test_virtual_order_and_virtuality_changes(self):
        for platform in ("windowsx86-64", "x86_64"):
            with self.subTest(platform=platform):
                old = self.build(
                    self.root / platform / "old",
                    platform,
                    fields="virtual int first() = 0; virtual int second() = 0; int a;",
                )
                for label, fields in [
                    (
                        "reordered",
                        "virtual int second() = 0; virtual int first() = 0; int a;",
                    ),
                    ("nonvirtual", "int first(); virtual int second() = 0; int a;"),
                ]:
                    new = self.build(
                        self.root / platform / label, platform, fields=fields
                    )
                    log = self.root / (platform + label + ".txt")
                    self.assertEqual(
                        compatibility.compare_debug_types(old, new, log),
                        "changed",
                        log.read_text(),
                    )
                    self.assertIn("'class Layout' changed", log.read_text())

    def test_vendor_shared_and_static_desktop_symbols(self):
        for platform in ("windowsx86-64", "x86_64"):
            with self.subTest(platform=platform):
                folder = self.root / platform
                old = self.build(folder / "old", platform)
                new = self.build(folder / "new", platform, changed=True)
                log = folder / "layout.txt"
                self.assertEqual(
                    compatibility.compare_debug_types(old, new, log), "changed"
                )
                source = folder / "vendor.cpp"
                source.write_text(
                    "struct Layout;\n#ifdef _WIN32\n__declspec(dllimport)\n#endif\nint consume(Layout*);\n#ifdef _WIN32\n__declspec(dllexport)\n#endif\nint caller(Layout* p) { return consume(p); }"
                )
                obj = folder / "vendor.o"
                shared = folder / (
                    "vendor.dll" if platform.startswith("windows") else "vendor.dylib"
                )
                static = folder / "vendor.a"
                triple = (
                    "x86_64-pc-windows-msvc"
                    if platform.startswith("windows")
                    else "x86_64-apple-macos13"
                )
                run(
                    llvm_tool("clang"),
                    "--target=" + triple,
                    "-c",
                    str(source),
                    "-o",
                    str(obj),
                )
                if platform.startswith("windows"):
                    run(
                        llvm_tool("lld-link"),
                        "/dll",
                        "/noentry",
                        "/nodefaultlib",
                        "/out:" + str(shared),
                        str(obj),
                        str(old.with_suffix(".lib")),
                    )
                else:
                    run(
                        llvm_tool("ld64.lld"),
                        "-dylib",
                        "-arch",
                        platform,
                        "-platform_version",
                        "macos",
                        "13.0",
                        "13.0",
                        "-undefined",
                        "dynamic_lookup",
                        "-o",
                        str(shared),
                        str(obj),
                    )
                run(llvm_tool("llvm-ar"), "rcs", str(static), str(obj))
                archive = folder / "vendor.zip"
                with zipfile.ZipFile(archive, "w") as contents:
                    contents.write(shared, shared.name)
                    contents.write(static, static.name)
                artifact = (
                    {"path": archive.name, "manifests": ["vendor.json"]},
                    archive,
                )
                architecture = None if platform.startswith("windows") else platform
                results = [{"library": "api", "status": "changed", "report": log.name}]
                result = native.trace_native(
                    [artifact],
                    {"api": old},
                    {"api": new},
                    results,
                    folder,
                    folder,
                    architecture,
                )
                self.assertFalse(result["errors"], result)
                self.assertEqual(len(result["findings"]), 2)
                removed = native.trace_native(
                    [artifact], {"api": old}, {}, [], folder, folder, architecture
                )
                self.assertEqual(len(removed["findings"]), 2)
                self.assertTrue(
                    all(
                        f["references"][0]["kind"] == "removed symbol"
                        for f in removed["findings"]
                    )
                )

    def test_missing_pdb_is_incomplete(self):
        old = self.build(self.root / "old", "windowsx86-64")
        new = self.build(self.root / "new", "windowsx86-64")
        new.with_suffix(".pdb").unlink()
        self.assertEqual(
            compatibility.compare_debug_types(old, new, self.root / "missing.txt"),
            "error",
        )

    def test_universal_slices_are_separate_and_required(self):
        binaries = [self.build(self.root / arch, arch) for arch in ("x86_64", "arm64")]
        universal = self.root / "universal.dylib"
        run(
            llvm_tool("llvm-lipo"),
            "-create",
            *map(str, binaries),
            "-output",
            str(universal),
        )
        debug = Path(str(universal) + ".debug")
        run(
            llvm_tool("llvm-lipo"),
            "-create",
            *[str(p) + ".debug" for p in binaries],
            "-output",
            str(debug),
        )
        (self.root / "native").mkdir()
        with zipfile.ZipFile(self.root / "native/plugin.zip", "w") as archive:
            archive.write(universal, "osx/universal/shared/libplugin.dylib")
            archive.write(debug, "osx/universal/shared/libplugin.dylib.debug")
        for arch in ("x86_64", "arm64"):
            result = compatibility.native_libraries(
                self.root, self.root / ("extracted-" + arch), arch
            )
            library = next(iter(result.values()))
            self.assertEqual(
                run(llvm_tool("llvm-lipo"), "-archs", str(library)).strip(), arch
            )
        with zipfile.ZipFile(self.root / "native/plugin.zip", "w") as archive:
            archive.write(binaries[0], "osx/universal/shared/libplugin.dylib")
        with self.assertRaisesRegex(ValueError, "Missing arm64"):
            compatibility.native_libraries(
                self.root, self.root / "missing-arm", "arm64"
            )


@unittest.skipUnless(
    shutil.which("javac") and shutil.which("g++"),
    "Install Java and C++ compilers for JNI fixtures",
)
class JniTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)

    def build(self, folder, changed=False):
        (folder / "java").mkdir(parents=True)
        source = folder / "Hidden.java"
        source.write_text(
            "package api; class Hidden { private "
            + ("long" if changed else "int")
            + " value; private native "
            + ("long" if changed else "int")
            + " call(int x); }"
        )
        run("javac", "-d", str(folder / "classes"), str(source))
        run(
            "jar",
            "cf",
            str(folder / "java/api.jar"),
            "-C",
            str(folder / "classes"),
            ".",
        )
        cpp = folder / "jni.cpp"
        # No JVM is loaded: these are linkable fixture exports and literal references only.
        symbol = "renamed" if changed else "Java_api_Hidden_call"
        cpp.write_text(
            'extern "C" int ' + symbol + "(void*, void*, int x) {return x;}\n"
            'extern "C" const char* lookup() {return "api/Hidden\\0value\\0I\\0";}\n'
        )
        binary = folder / "libjni.so"
        run("g++", "-shared", "-fPIC", "-g", str(cpp), "-o", str(binary))
        return binary

    def test_private_members_jni_names_and_native_declarations(self):
        base, head = self.root / "base", self.root / "head"
        old, new = self.build(base), self.build(head, changed=True)
        archive = self.root / "vendor.zip"
        with zipfile.ZipFile(archive, "w") as contents:
            contents.write(old, "linux/x86-64/shared/libvendor.so")
        artifact = {"path": "vendor.zip", "manifests": ["vendor.json"]}
        result = jni.compare(
            base,
            head,
            {"jni/libjni.so": old},
            {"jni/libjni.so": new},
            [(artifact, archive)],
            self.root,
        )
        self.assertFalse(result["errors"])
        kinds = {f["kind"] for f in result["findings"]}
        self.assertIn("JNI export removed from library", kinds)
        self.assertIn("Java native declaration changed", kinds)
        vendor = next(
            f
            for f in result["findings"]
            if f["kind"] == "vendor native Java references"
        )
        self.assertEqual(vendor["references"][0]["name"], "value")
        self.assertEqual(vendor["references"][0]["descriptor"], "I")
        # A preexisting missing class is not newly attributed to this change.
        self.assertFalse(
            jni.literal_findings(
                b"api/AlreadyMissing\0value\0I\0",
                jni.java_changes(jni.classes(base), jni.classes(head)),
            )
        )
        clean = jni.compare(
            base,
            base,
            {"jni/libjni.so": old},
            {"jni/libjni.so": old},
            [(artifact, archive)],
            self.root,
        )
        self.assertFalse(clean["findings"])

    def test_removed_java_class_literal(self):
        old = {"api/Gone": {"flags": 0, "member_flags": {}}}
        changes = jni.java_changes(old, {})
        self.assertEqual(
            jni.literal_findings(b"api/Gone\0", changes)[0]["kind"], "class removed"
        )
