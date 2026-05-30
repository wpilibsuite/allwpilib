import time
import typing as T

from ._simulation import getNextNotifierTimeout, getProgramStarted


def waitForProgramStart(
    timeout: T.Optional[float] = None,
    delta: float = 0.001,
    waitForFirstNotifier: bool = True,
):
    """
    Polls robot program and returns when it has reported that it started

    :param timeout: Amount of time to wait
    :param delta:   Amount of time to sleep between checks
    :param waitForFirstNotifier: Wait for the first notifier alarm to be armed
    """

    # This is basically the same thing that the C version of this function
    # does. Implemented in python so that CTRL-C works.

    until = None
    if timeout and timeout > 0:
        until = time.monotonic() + timeout
    while not getProgramStarted() or (
        waitForFirstNotifier and getNextNotifierTimeout() == 0xFFFFFFFFFFFFFFFF
    ):
        if until is not None and time.monotonic() > until:
            raise TimeoutError("Program did not start")
        time.sleep(delta)
