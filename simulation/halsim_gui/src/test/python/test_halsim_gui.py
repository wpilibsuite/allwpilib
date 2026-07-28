import ctypes
import ctypes.util
import pathlib
import sys

INIT_MODULE = "native.halsim_gui._init_robotpy_native_halsim_gui"

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
    from halsim_gui.main import _get_extension_path

    sys.modules.pop(INIT_MODULE, None)

    ext = pathlib.Path(_get_extension_path())
    assert INIT_MODULE not in sys.modules
    assert ext.is_file()
    assert ext.suffix in (".dll", ".dylib", ".so")
    ctypes.CDLL(str(ext))

    assert base.load_extension
