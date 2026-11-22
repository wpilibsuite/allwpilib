from collections.abc import Callable

# Type Aliases
type FloatSupplier = Callable[[], float]
type FloatOrFloatSupplier = float | FloatSupplier
