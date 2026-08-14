from typing import Callable, Union

from typing_extensions import TypeAlias

# Type Aliases
FloatSupplier: TypeAlias = Callable[[], float]
FloatOrFloatSupplier: TypeAlias = Union[float, Callable[[], float]]
