import logging
import os
from os.path import abspath, dirname, join

logger = logging.getLogger("halsim_ws.server")


def loadExtension():
    try:
        import hal
    except ImportError as e:
        # really, should never happen...
        raise ImportError("you must install robotpy-hal!") from e

    from ..version import version

    logger.info("WPILib HAL Simulation websim server %s", version)

    root = join(abspath(dirname(__file__)), "lib")
    ext = join(root, os.listdir(root)[0])
    retval = hal.loadOneExtension(ext)
    if retval != 0:
        logger.warn("loading extension may have failed (error=%d)", retval)
