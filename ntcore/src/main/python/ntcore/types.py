from collections.abc import Sequence

ValueT = (
    bool
    | int
    | float
    | str
    | bytes
    | Sequence[bool]
    | Sequence[int]
    | Sequence[float]
    | Sequence[str]
)
