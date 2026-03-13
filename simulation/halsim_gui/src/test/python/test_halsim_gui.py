import ctypes
import ctypes.util
import pathlib
import sys

import pytest


@pytest.mark.skipif(
    sys.platform == "linux" and not ctypes.util.find_library("X11"),
    reason="X11 not installed",
)
def test_halsim_gui():
    # dependencies
    import wpinet
    import hal
    import wpimath
    import ntcore

    import halsim_gui as base

    loaded = 0
    for fname in (pathlib.Path(base.__file__).parent / "lib").iterdir():
        if fname.is_file() and fname.suffix in (".dll", ".dylib", ".so"):
            ctypes.CDLL(str(fname))
            loaded += 1

    assert loaded
