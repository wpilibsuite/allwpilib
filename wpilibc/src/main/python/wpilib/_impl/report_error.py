import hal
import sys
import traceback
import logging
import inspect

robotpy_logger = logging.getLogger("robotpy")
user_logger = logging.getLogger("your.robot")


def reportErrorInternal(
    error: str,
    printTrace: bool = False,
    fromUser: bool = False,
    isWarning: bool = True,
    code: int = 1,
) -> None:
    traceString = ""

    if fromUser:
        log = user_logger
    else:
        log = robotpy_logger

    if printTrace:
        exc_info = sys.exc_info()

        exc = exc_info[0]
        if exc is None:
            tb = traceback.extract_stack()[:-2]
        else:
            tb = traceback.extract_tb(exc_info[2])

        locString = "%s.%s:%s" % (tb[-1][0], tb[-1][1], tb[-1][2])

        trc = "Traceback (most recent call last):\n"
        stackstr = trc + "".join(traceback.format_list(tb))
        if exc is not None:
            stackstr += "  " + ("".join(traceback.format_exception(*exc_info))).lstrip(
                trc
            )
        traceString += "\n" + stackstr

        if exc is None:
            log.error(error + "\n" + traceString)
        else:
            log.error(error, exc_info=exc_info)
    else:
        if isWarning:
            log.warning(error)
        else:
            log.error(error)

        try:
            frame = inspect.stack(context=2)[-1]
            locString = f"{frame.filename}:{frame.lineno}"
        except Exception:
            locString = "<unknown location>"

    if not hal.__hal_simulation__:
        hal.sendError(
            not isWarning,
            code,
            False,
            error,
            locString,
            traceString,
            True,
        )


def reportError(error: str, printTrace: bool = False) -> None:
    """
    Report error to Driver Station, and also prints error to ``sys.stderr``.
    Optionally appends stack trace to error message.

    :param error: message to show
    :param printTrace: If True, appends stack trace to error string
    """
    reportErrorInternal(error, printTrace, fromUser=True)


def reportWarning(error: str, printTrace: bool = False) -> None:
    """
    Report warning to Driver Station, and also prints error to ``sys.stderr``.
    Optionally appends stack trace to error message.

    :param error: message to show
    :param printTrace: If True, appends stack trace to error string
    """
    reportErrorInternal(error, printTrace, fromUser=True, isWarning=True)
