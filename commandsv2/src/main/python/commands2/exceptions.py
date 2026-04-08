# notrack

import typing


class IllegalCommandUse(Exception):
    """
    This exception is raised when a command is used in a way that it shouldn't be.

    You shouldn't try to catch this exception, if it occurs it means your code is
    doing something it probably shouldn't be doing
    """

    def __init__(self, msg: str, **kwargs: typing.Any) -> None:
        if kwargs:
            args_repr = ", ".join(f"{k}={v!r}" for k, v in kwargs.items())
            msg = f"{msg} ({args_repr})"

        super().__init__(msg)
