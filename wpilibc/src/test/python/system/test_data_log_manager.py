import pathlib
import wpilib
import pytest
import sys
from wpilib.simulation import AlertSim
from wpiutil import Alert


@pytest.mark.skipif(sys.platform == "darwin", reason="DataLogManager crashes on exit")
def test_get_log(tmp_path: pathlib.Path):
    log_dir = tmp_path / "wpilogs"
    log_dir.mkdir()
    for _ in range(2):
        wpilib.DataLogManager.start(str(log_dir))
        try:
            log = wpilib.DataLogManager.get_log()
            assert log is not None
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
