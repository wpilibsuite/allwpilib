import subprocess
import sys
import textwrap

import pytest


@pytest.mark.parametrize(
    "setup",
    [
        'telemetry.TelemetryRegistry.register_backend("/test", Backend())',
        'telemetry.TelemetryRegistry.register_backend("/test", Backend()); '
        'telemetry.log("/test/value", 1)',
        'telemetry.TelemetryRegistry.register_backend("/test", '
        "telemetry.MultiTelemetryBackend([Backend()])); "
        'telemetry.log("/test/value", 1)',
        "telemetry.TelemetryRegistry.set_report_warning(lambda path, msg: None)",
    ],
    ids=["backend", "cached-entry", "multi-backend", "warning-callback"],
)
def test_python_registry_objects_are_safe_at_shutdown(setup):
    # A subprocess is necessary: normal test cleanup masks registry objects
    # being destroyed by C++ static destructors after Python has finalized.
    code = textwrap.dedent("""
        import telemetry

        class Entry(telemetry.TelemetryEntry):
            def log_int64(self, value, timestamp):
                pass

        class Backend(telemetry.TelemetryBackend):
            def get_entry(self, path):
                return Entry()
        """)
    code += "\n" + setup

    result = subprocess.run(
        [sys.executable, "-c", code],
        capture_output=True,
        text=True,
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    # Cleanup must not emit ignored exceptions during interpreter shutdown.
    assert result.stderr == ""
