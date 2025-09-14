from . import exceptions, _init__wpiHal, _wpiHal

# Always initialize HAL here, disable extension notice because we'll handle
# that for users
_sse = getattr(_wpiHal, "setShowExtensionsNotFoundMessages", None)
if _sse:
    _wpiHal.setShowExtensionsNotFoundMessages(False)

_wpiHal.initialize(500, 0)
