# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Exercise diagnostics with real processes, including silent exits and signals."""

import json
import os
import signal
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

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
        return (
            code,
            json.loads(log.with_suffix(".process.json").read_text()),
            log.read_text(),
        )

    def test_output_and_abi_bitmask_are_preserved(self):
        code, data, log = self.run_python(
            "print('ABI changed', flush=True); raise SystemExit(12)"
        )
        self.assertEqual(code, 12)
        self.assertEqual(data["returncode"], 12)
        self.assertFalse(data["timed_out"])
        self.assertGreater(data["elapsed_seconds"], 0)
        self.assertIn("ABI changed", log)
        self.assertIn("exit 12", log)

    def test_failed_start_is_diagnosed(self):
        log = self.root / "missing.txt"
        self.assertEqual(process.run_tool([str(self.root / "missing")], log), -1)
        data = json.loads(log.with_suffix(".process.json").read_text())
        self.assertIsNone(data["returncode"])
        self.assertIn("error", data)
        self.assertIn("Process diagnostics", log.read_text())

    def test_timeout_is_explicit(self):
        code, data, log = self.run_python("import time; time.sleep(60)", timeout=0.15)
        self.assertEqual(code, -1)
        self.assertTrue(data["timed_out"])
        self.assertIn("timeout after 0.15s", log)

    @unittest.skipUnless(os.name == "posix", "POSIX signals")
    def test_silent_signal_and_oom_counter_are_retained(self):
        with patch.object(
            process,
            "memory_events",
            side_effect=[
                {"path": "/group/memory.events", "counts": {"oom_kill": 2}},
                {"path": "/group/memory.events", "counts": {"oom_kill": 3}},
            ],
        ):
            code, data, log = self.run_python(
                "import os, signal; os.kill(os.getpid(), signal.SIGKILL)"
            )
        self.assertEqual(code, -signal.SIGKILL)
        self.assertEqual(data["signal"], "SIGKILL")
        self.assertFalse(data["timed_out"])
        self.assertEqual(data["cgroup_memory_events_delta"]["oom_kill"], 1)
        self.assertIn("SIGKILL", log)
        self.assertIn("may include other processes", log)

    @unittest.skipUnless(hasattr(os, "wait4"), "wait4 resource accounting")
    def test_peak_memory_is_per_process_not_previous_high_water_mark(self):
        _, large, _ = self.run_python(
            "storage = bytearray(64 * 1024 * 1024)", name="large"
        )
        _, small, _ = self.run_python("pass", name="small")
        self.assertGreater(
            large["peak_rss_bytes"], small["peak_rss_bytes"] + 32 * 1024**2
        )
        self.assertGreaterEqual(small["user_cpu_seconds"], 0)
        self.assertGreaterEqual(small["system_cpu_seconds"], 0)

    def test_missing_resource_support_still_runs_tool(self):
        # The Windows path has no wait4; diagnostics must not prevent analysis.
        with patch.dict(os.__dict__):
            os.__dict__.pop("wait4", None)
            code, data, _ = self.run_python("pass")
            self.assertEqual(code, 0)
            self.assertNotIn("peak_rss_bytes", data)

    def test_failed_stage_still_records_duration(self):
        timings = {}
        with self.assertRaises(ValueError), process.timed("stage", timings):
            raise ValueError("failure")
        self.assertIn("stage", timings)


if __name__ == "__main__":
    unittest.main()
