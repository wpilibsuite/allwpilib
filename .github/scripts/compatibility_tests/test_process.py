# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Exercise tool failures, including silent exits and signals."""

import os
import signal
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import compatibility_process as process


class ProcessTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)

    def run_python(self, source, name="tool", **kwargs):
        log = self.root / f"{name}.txt"
        code = process.run_tool([sys.executable, "-c", source], log, **kwargs)
        return code, log.read_text()

    def test_output_and_abi_bitmask_are_preserved(self):
        code, log = self.run_python(
            "print('ABI changed', flush=True); raise SystemExit(12)"
        )
        self.assertEqual(code, 12)
        self.assertIn("ABI changed", log)

    def test_failed_start_is_diagnosed(self):
        log = self.root / "missing.txt"
        self.assertEqual(process.run_tool([str(self.root / "missing")], log), -1)
        self.assertIn("Tool failed to start", log.read_text())

    def test_timeout_is_explicit(self):
        code, log = self.run_python("import time; time.sleep(60)", timeout=0.15)
        self.assertEqual(code, -1)
        self.assertIn("timed out after 0.15s", log)

    @unittest.skipUnless(os.name == "posix", "POSIX signals")
    def test_silent_signal_is_retained(self):
        code, log = self.run_python(
            "import os, signal; os.kill(os.getpid(), signal.SIGKILL)"
        )
        self.assertEqual(code, -signal.SIGKILL)
        self.assertIn("SIGKILL", log)


if __name__ == "__main__":
    unittest.main()
