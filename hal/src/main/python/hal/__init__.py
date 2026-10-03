from .version import version as __version__

# Only needed for side effects
from . import _initialize
from .exceptions import HALError

from . import _init__wpi_hal, _wpi_hal
from ._wpi_hal import *

from ._wpi_hal import __hal_simulation__

del _init__wpi_hal
