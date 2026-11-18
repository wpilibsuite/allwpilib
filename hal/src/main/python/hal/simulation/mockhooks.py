import threading
import time

from ._simulation import wait_for_program_start as _wait_for_program_start


def wait_for_program_start(
    timeout: float | None = None,
    delta: float = 0.001,
    wait_for_first_notifier: bool = True,
):
    """
    Waits for the robot program to report that it started.

    Delegates to HALSIM_WaitForProgramStart so the wait matches C++: a
    notifier alarm must be set *after* program start (a stale notifier
    cannot satisfy the check). The C++ call releases the GIL. timeout and
    CTRL-C are handled on this thread so this remains interruptible.

    :param timeout: Amount of time to wait
    :param delta:   Amount of time to sleep between checks
    :param wait_for_first_notifier: Wait for the first notifier alarm to be armed
    """

    done = threading.Event()

    def _run():
        try:
            _wait_for_program_start(wait_for_first_notifier)
        finally:
            done.set()

    threading.Thread(target=_run, daemon=True).start()

    until = None
    if timeout and timeout > 0:
        until = time.monotonic() + timeout
    while not done.is_set():
        if until is not None and time.monotonic() > until:
            raise TimeoutError("Program did not start")
        # Wake periodically so CTRL-C can be delivered on this thread.
        done.wait(timeout=delta)
