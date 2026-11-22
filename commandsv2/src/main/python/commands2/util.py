# notrack
from __future__ import annotations

from collections.abc import Iterable

from .command import Command


def flatten_args_commands(
    *commands: Command | Iterable[Command],
) -> tuple[Command, ...]:
    flattened_commands: list[Command] = []
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
