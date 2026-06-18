import pathlib
import wpilib
import pytest
import sys


@pytest.mark.skipif(sys.platform == "darwin", reason="DataLogManager crashes on exit")
def test_get_log(tmp_path: pathlib.Path):
    log_dir = tmp_path / "wpilogs"
    log_dir.mkdir()
    wpilib.DataLogManager.start(str(log_dir))
    log = wpilib.DataLogManager.getLog()
    assert log is not None
