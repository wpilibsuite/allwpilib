import pathlib
import wpilib
import pytest
import sys
import time
from wpilib.simulation import AlertSim
from wpiutil import Alert


@pytest.mark.skipif(sys.platform == "darwin", reason="DataLogManager crashes on exit")
def test_get_log(tmp_path: pathlib.Path):
    log_dir = tmp_path / "wpilogs"
    log_dir.mkdir()
    matches = [
        log_dir / f"WPILIB_20261022_123456_Event_{suffix}.wpilog"
        for suffix in ("P1", "Q123", "E13", "T4", "P1.2", "Q123.12", "E13.3", "T4.2")
    ]
    for match in matches:
        match.write_bytes(b"match log")
    temporary = log_dir / "WPILIB_TBD_1234567890abcdef.wpilog"
    temporary.touch()
    for _ in range(2):
        wpilib.DataLogManager.start(str(log_dir))
        try:
            log = wpilib.DataLogManager.get_log()
            assert log is not None
            assert not temporary.exists()
            assert all(match.read_bytes() == b"match log" for match in matches)
            assert wpilib.DataLogManager.get_log_dir() == str(log_dir)
            alerts = {
                a.id: a for a in AlertSim.get_all() if a.group == "DataLogManager"
            }
            assert set(alerts) == {"internalStorage", "lowSpace"}
            assert alerts["internalStorage"].level == Alert.Level.LOW
            assert not alerts["internalStorage"].is_active()
            assert alerts["lowSpace"].level == Alert.Level.MEDIUM
        finally:
            wpilib.DataLogManager.stop()
        assert not any(a.group == "DataLogManager" for a in AlertSim.get_all())
    assert all(match.read_bytes() == b"match log" for match in matches)

@pytest.mark.skipif(sys.platform == "darwin", reason="DataLogManager crashes on exit")
def test_late_custom_dir(tmp_path: pathlib.Path):
    # The custom directory does not exist yet: logging must fall back to the
    # default location instead of dropping data, then transfer once it appears.
    custom = tmp_path / "custom"
    fallback_dir = None
    try:
        wpilib.DataLogManager.start(str(custom))
        fallback_dir = pathlib.Path(wpilib.DataLogManager.get_log_dir())
        assert fallback_dir != custom
        wpilib.DataLogManager.log("hello")
        custom.mkdir()
        deadline = time.monotonic() + 15
        while pathlib.Path(wpilib.DataLogManager.get_log_dir()) != custom:
            assert time.monotonic() < deadline
            time.sleep(0.1)
        moved = [p for p in custom.iterdir() if p.suffix == ".wpilog"]
        assert moved
        assert not any((fallback_dir / p.name).exists() for p in moved)
    finally:
        wpilib.DataLogManager.stop()
        if fallback_dir is not None:
            for p in fallback_dir.iterdir():
                if p.name.startswith("WPILIB_TBD_") and p.suffix == ".wpilog":
                    p.unlink()
