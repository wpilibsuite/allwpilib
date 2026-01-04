import logging
import os
from os.path import abspath, dirname, join

logger = logging.getLogger("halsim_gui")


def loadExtension():
    try:
        import hal
    except ImportError as e:
        # really, should never happen...
        raise ImportError("you must install robotpy-hal!") from e

    from .version import version

    logger.info("WPILib HAL Simulation %s", version)

    root = join(abspath(dirname(__file__)), "lib")
    ext = join(root, os.listdir(root)[0])
    retval = hal.loadOneExtension(ext)
    if retval != 0:
        logger.warn("loading extension may have failed (error=%d)", retval)

    from ._ext._halsim_gui_ext import _kill_on_signal

    _kill_on_signal()
