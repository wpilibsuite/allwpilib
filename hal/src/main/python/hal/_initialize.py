from . import _init__wpiHal, _wpiHal, exceptions  # noqa

# Always initialize HAL here, disable extension notice because we'll handle
# that for users
_sse = getattr(_wpiHal, "setShowExtensionsNotFoundMessages", None)
if _sse:
    _wpiHal.setShowExtensionsNotFoundMessages(False)

_wpiHal.initialize(500, 0)
