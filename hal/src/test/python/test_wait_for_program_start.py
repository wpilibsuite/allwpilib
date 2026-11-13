import threading

import pytest

import hal
import hal.simulation as halsim

# HALSIM_GetNextNotifierTimeout returns int64 max when no alarm is armed.
NO_ALARM = 0x7FFFFFFFFFFFFFFF


@pytest.fixture
def program_start_sim():
    paused_before = halsim.is_timing_paused()
    started_before = halsim.get_program_started()
    if not paused_before:
        halsim.pause_timing()
    halsim.set_program_started(False)
    try:
        yield
    finally:
        halsim.set_program_started(started_before)
        if not paused_before:
            halsim.resume_timing()


def test_wait_for_program_start_ignores_stale_notifier(program_start_sim):
    """A notifier armed before program start must not satisfy the wait.

    The old Python wait_for_program_start only checked
    get_next_notifier_timeout() != uint64-max, so a stale motor-safety-like
    notifier made it return as soon as the program-started flag was set.
    C++ HALSIM_WaitForProgramStart snapshots the alarm-set count in
    SetProgramStarted and waits for a later alarm.
    """
    notifier = None
    try:
        notifier, status = hal.create_notifier()
        assert status == 0
        # Far-future relative alarm; timing is paused so it will not fire.
        assert hal.set_notifier_alarm(notifier, 10_000_000_000, 0, False, True) == 0
        assert halsim.get_next_notifier_timeout() != NO_ALARM

        # Snapshots the alarm-set count, including the stale alarm above.
        halsim.set_program_started(True)

        done = threading.Event()
        errors: list[BaseException] = []

        def _wait():
            try:
                halsim.wait_for_program_start(timeout=10.0)
            except BaseException as exc:
                errors.append(exc)
            finally:
                done.set()

        threading.Thread(target=_wait, daemon=True).start()

        # Old implementation returns immediately once the program-started flag
        # is set and a (possibly stale) notifier timeout is present.
        assert not done.wait(timeout=0.05)

        # An alarm set after program start should unblock the wait.
        assert hal.set_notifier_alarm(notifier, 20_000_000_000, 0, False, True) == 0
        assert done.wait(timeout=10.0)
        assert errors == []
    finally:
        if notifier is not None:
            hal.destroy_notifier(notifier)
