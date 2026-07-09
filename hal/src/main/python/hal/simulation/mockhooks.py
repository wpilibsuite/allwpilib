import time
import typing as T

from ._simulation import get_next_notifier_timeout, get_program_started


def wait_for_program_start(
    timeout: T.Optional[float] = None,
    delta: float = 0.001,
    wait_for_first_notifier: bool = True,
):
    """
    Polls robot program and returns when it has reported that it started

    :param timeout: Amount of time to wait
    :param delta:   Amount of time to sleep between checks
    :param wait_for_first_notifier: Wait for the first notifier alarm to be armed
    """

    # This is basically the same thing that the C version of this function
    # does. Implemented in python so that CTRL-C works.

    until = None
    if timeout and timeout > 0:
        until = time.monotonic() + timeout
    while not get_program_started() or (
        wait_for_first_notifier and get_next_notifier_timeout() == 0xFFFFFFFFFFFFFFFF
    ):
        if until is not None and time.monotonic() > until:
            raise TimeoutError("Program did not start")
        time.sleep(delta)
