import logging
import os
from os.path import abspath, dirname, join

logger = logging.getLogger("halsim_ds_socket")


def load_extension():
    try:
        import hal
    except ImportError as e:
        # really, should never happen...
        raise ImportError("you must install robotpy-hal!") from e

    from .version import version

    logger.info("WPILib HAL Simulation DS Socket Extension %s", version)

    root = join(abspath(dirname(__file__)), "lib")
    ext = join(root, os.listdir(root)[0])

    # Load MrcLib globally before loading the extension that links to it.
    import native.wpihal._init_robotpy_native_mrclib  # noqa: F401

    retval = hal.load_one_extension(ext)
    if retval != 0:
        logger.warn("loading extension may have failed (error=%d)", retval)
