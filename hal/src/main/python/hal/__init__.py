from .version import version as __version__

# Only needed for side effects
from . import _initialize
from .exceptions import HALError

from . import _init__wpiHal
from ._wpiHal import *

from ._wpiHal import __hal_simulation__

del _init__wpiHal
