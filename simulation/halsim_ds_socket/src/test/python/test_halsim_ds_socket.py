import ctypes
import sys
import types


def test_halsim_ds_socket(monkeypatch):
    # dependencies
    import native.wpihal._init_robotpy_native_wpihal
    import native.wpinet._init_robotpy_native_wpinet

    import halsim_ds_socket as base

    mrclib_init = "native.wpihal._init_robotpy_native_mrclib"
    assert mrclib_init not in sys.modules

    loaded = []

    def load_one_extension(fname):
        ctypes.CDLL(fname)
        loaded.append(fname)
        return 0

    hal = types.ModuleType("hal")
    hal.load_one_extension = load_one_extension
    monkeypatch.setitem(sys.modules, "hal", hal)

    base.load_extension()

    assert len(loaded) == 1
    assert mrclib_init in sys.modules
