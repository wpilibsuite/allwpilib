# notrack
from __future__ import annotations

from typing import Iterable, List, Tuple, Union

from .command import Command


def flatten_args_commands(
    *commands: Union[Command, Iterable[Command]]
) -> Tuple[Command, ...]:
    flattened_commands: List[Command] = []
    for command in commands:
        if isinstance(command, Command):
            flattened_commands.append(command)
        elif isinstance(command, Iterable):
            flattened_commands.extend(flatten_args_commands(*command))
    return tuple(flattened_commands)


def format_args_kwargs(*args, **kwargs) -> str:
    return ", ".join(
        [repr(arg) for arg in args]
        + [f"{key}={repr(value)}" for key, value in kwargs.items()]
    )
