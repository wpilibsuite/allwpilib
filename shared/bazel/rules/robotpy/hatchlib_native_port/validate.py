import typing

import validobj.validation
from validobj import errors

T = typing.TypeVar("T")


class ValidationError(Exception):
    pass


def _convert_validation_error(
    fname, ve: errors.ValidationError, prefix: str
) -> ValidationError:
    locs = []
    msgs = []

    e: typing.Optional[BaseException] = ve
    while e is not None:

        if isinstance(e, errors.WrongFieldError):
            locs.append(f".{e.wrong_field}")
        elif isinstance(e, errors.WrongListItemError):
            locs.append(f"[{e.wrong_index}]")
        else:
            msgs.append(str(e))

        e = e.__cause__

    loc = "".join(locs)
    if loc.startswith("."):
        loc = loc[1:]
    msg = "\n  ".join(msgs)
    vmsg = f"{fname}: {prefix}{loc}:\n  {msg}"
    return ValidationError(vmsg)


def parse_input(value: typing.Any, spec: typing.Type[T], fname, prefix: str) -> T:
    try:
        return validobj.validation.parse_input(value, spec)
    except errors.ValidationError as ve:
        raise _convert_validation_error(fname, ve, prefix) from None
