import hal
import sys
import traceback
import logging
import inspect

robotpy_logger = logging.getLogger("robotpy")
user_logger = logging.getLogger("your.robot")


def report_error_internal(
    error: str,
    print_trace: bool = False,
    from_user: bool = False,
    is_warning: bool = True,
    code: int = 1,
) -> None:
    trace_string = ""

    if from_user:
        log = user_logger
    else:
        log = robotpy_logger

    if print_trace:
        exc_info = sys.exc_info()

        exc = exc_info[0]
        if exc is None:
            tb = traceback.extract_stack()[:-2]
        else:
            tb = traceback.extract_tb(exc_info[2])

        loc_string = "%s.%s:%s" % (tb[-1][0], tb[-1][1], tb[-1][2])

        trc = "Traceback (most recent call last):\n"
        stackstr = trc + "".join(traceback.format_list(tb))
        if exc is not None:
            stackstr += "  " + ("".join(traceback.format_exception(*exc_info))).lstrip(
                trc
            )
        trace_string += "\n" + stackstr

        if exc is None:
            log.error(error + "\n" + trace_string)
        else:
            log.error(error, exc_info=exc_info)
    else:
        if is_warning:
            log.warning(error)
        else:
            log.error(error)

        try:
            frame = inspect.stack(context=2)[-1]
            loc_string = f"{frame.filename}:{frame.lineno}"
        except Exception:
            loc_string = "<unknown location>"

    if not hal.__hal_simulation__:
        hal.send_error(
            not is_warning,
            code,
            error,
            loc_string,
            trace_string,
            True,
        )


def report_error(error: str, print_trace: bool = False) -> None:
    """
    Report error to Driver Station, and also prints error to ``sys.stderr``.
    Optionally appends stack trace to error message.

    :param error: message to show
    :param print_trace: If True, appends stack trace to error string
    """
    report_error_internal(error, print_trace, from_user=True)


def report_warning(error: str, print_trace: bool = False) -> None:
    """
    Report warning to Driver Station, and also prints error to ``sys.stderr``.
    Optionally appends stack trace to error message.

    :param error: message to show
    :param print_trace: If True, appends stack trace to error string
    """
    report_error_internal(error, print_trace, from_user=True, is_warning=True)
