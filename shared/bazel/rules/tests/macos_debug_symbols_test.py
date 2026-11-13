"""Verify that published macOS debug files are usable without build objects."""

import re
import subprocess
import sys
import tempfile
import unittest
import zipfile
from pathlib import Path


def dwarfdump(*args):
    return subprocess.check_output(
        ["/usr/bin/xcrun", "dwarfdump", *map(str, args)], text=True
    )


def uuids(path):
    return set(re.findall(r"UUID: (\S+) \(([^)]+)\)", dwarfdump("--uuid", path)))


class MacosDebugSymbolsTest(unittest.TestCase):
    def test_published_debug_symbols(self):
        for archive in ARCHIVES:
            with self.subTest(archive=archive), tempfile.TemporaryDirectory() as tmp:
                # Only the published files are extracted; no objects or archives
                # are available to reconstruct missing debug information.
                with zipfile.ZipFile(archive) as package:
                    package.extractall(tmp)
                shared = Path(tmp) / "osx/universal/shared"
                library = shared / "libfixture.lib"
                debug = shared / "libfixture.lib.debug"
                self.assertTrue(library.is_file())
                self.assertTrue(debug.is_file())
                library_uuids = uuids(library)
                self.assertEqual(
                    {arch for _, arch in library_uuids}, {"arm64", "x86_64"}
                )
                self.assertEqual(uuids(debug), library_uuids)
                for arch in ("arm64", "x86_64"):
                    info = dwarfdump("--arch", arch, "--debug-info", debug)
                    self.assertIn('"FixtureValue"', info)
                    self.assertIn('"DependencyValue"', info)
                dwarfdump("--verify", debug)


if __name__ == "__main__":
    ARCHIVES = sys.argv[1:]
    if len(ARCHIVES) != 2:
        raise ValueError("Expected optimized and debug publication ZIPs")
    unittest.main(argv=[sys.argv[0]])
