import importlib.util
import logging
import os
from os.path import join

logger = logging.getLogger("halsim_gui")


def _get_extension_path():
    spec = importlib.util.find_spec("native.halsim_gui")
    if spec is None or spec.submodule_search_locations is None:
        raise ImportError("you must install robotpy-native-halsim-gui!")

    suffixes = (".dll", ".dylib", ".so")
    for location in spec.submodule_search_locations:
        root = join(location, "lib")
        for fname in os.listdir(root):
            if fname.startswith(("halsim_gui", "libhalsim_gui")) and fname.endswith(
                suffixes
            ):
                return join(root, fname)

    raise FileNotFoundError("halsim_gui shared library was not found")


def load_extension():
    try:
        import hal
    except ImportError as e:
        # really, should never happen...
        raise ImportError("you must install robotpy-hal!") from e

    from .version import version

    logger.info("WPILib HAL Simulation %s", version)

    retval = hal.load_one_extension(_get_extension_path())
    if retval != 0:
        logger.warning("loading extension may have failed (error=%d)", retval)

    from ._ext._halsim_gui_ext import _kill_on_signal

    _kill_on_signal()
