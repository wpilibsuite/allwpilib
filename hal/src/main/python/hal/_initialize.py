from . import exceptions, _init__wpi_hal, _wpi_hal

# Always initialize HAL here, disable extension notice because we'll handle
# that for users
_sse = getattr(_wpi_hal, "set_show_extensions_not_found_messages", None)
if _sse:
    _wpi_hal.set_show_extensions_not_found_messages(False)

_wpi_hal.initialize()
