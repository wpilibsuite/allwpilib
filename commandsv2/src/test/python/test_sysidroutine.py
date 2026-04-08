import pytest
from unittest.mock import Mock, call, ANY
from wpilib.simulation import stepTiming, pauseTiming, resumeTiming
from wpimath.units import volts
from commands2 import Command, Subsystem
from commands2.sysid import SysIdRoutine
from wpilib.sysid import SysIdRoutineLog, State


class Mechanism(Subsystem):
    def recordState(self, state: State):
        pass

    def drive(self, voltage: volts):
        pass

    def log(self, log: SysIdRoutineLog):
        pass


@pytest.fixture
def mechanism():
    return Mock(spec=Mechanism)


@pytest.fixture
def sysid_routine(mechanism):
    return SysIdRoutine(
        SysIdRoutine.Config(recordState=mechanism.recordState),
        SysIdRoutine.Mechanism(mechanism.drive, mechanism.log, Subsystem()),
    )


@pytest.fixture
def quasistatic_forward(sysid_routine):
    return sysid_routine.quasistatic(SysIdRoutine.Direction.kForward)


@pytest.fixture
def quasistatic_reverse(sysid_routine):
    return sysid_routine.quasistatic(SysIdRoutine.Direction.kReverse)


@pytest.fixture
def dynamic_forward(sysid_routine):
    return sysid_routine.dynamic(SysIdRoutine.Direction.kForward)


@pytest.fixture
def dynamic_reverse(sysid_routine):
    return sysid_routine.dynamic(SysIdRoutine.Direction.kReverse)


@pytest.fixture(autouse=True)
def timing():
    pauseTiming()
    yield
    resumeTiming()


def run_command(command: Command):
    command.initialize()
    command.execute()
    stepTiming(1)
    command.execute()
    command.end(True)


def test_record_state_bookends_motor_logging(
    mechanism, quasistatic_forward, dynamic_forward
):
    run_command(quasistatic_forward)

    mechanism.assert_has_calls(
        [
            call.drive(ANY),
            call.log(ANY),
            call.recordState(State.kQuasistaticForward),
            call.drive(ANY),
            call.recordState(State.kNone),
        ],
        any_order=False,
    )

    mechanism.reset_mock()
    run_command(dynamic_forward)

    mechanism.assert_has_calls(
        [
            call.drive(ANY),
            call.log(ANY),
            call.recordState(State.kDynamicForward),
            call.drive(ANY),
            call.recordState(State.kNone),
        ],
        any_order=False,
    )


def test_tests_declare_correct_state(
    mechanism,
    quasistatic_forward,
    quasistatic_reverse,
    dynamic_forward,
    dynamic_reverse,
):
    run_command(quasistatic_forward)
    mechanism.recordState.assert_any_call(State.kQuasistaticForward)

    run_command(quasistatic_reverse)
    mechanism.recordState.assert_any_call(State.kQuasistaticReverse)

    run_command(dynamic_forward)
    mechanism.recordState.assert_any_call(State.kDynamicForward)

    run_command(dynamic_reverse)
    mechanism.recordState.assert_any_call(State.kDynamicReverse)


def test_tests_output_correct_voltage(
    mechanism,
    quasistatic_forward,
    quasistatic_reverse,
    dynamic_forward,
    dynamic_reverse,
):
    run_command(quasistatic_forward)

    mechanism.drive.assert_has_calls(
        [
            call(pytest.approx(1.0)),
            call(pytest.approx(0.0)),
        ],
        any_order=False,
    )

    mechanism.reset_mock()
    run_command(quasistatic_reverse)

    mechanism.drive.assert_has_calls(
        [
            call(pytest.approx(-1.0)),
            call(pytest.approx(0.0)),
        ],
        any_order=False,
    )

    mechanism.reset_mock()
    run_command(dynamic_forward)

    mechanism.drive.assert_has_calls(
        [
            call(pytest.approx(7.0)),
            call(pytest.approx(0.0)),
        ],
        any_order=False,
    )

    mechanism.reset_mock()
    run_command(dynamic_reverse)

    mechanism.drive.assert_has_calls(
        [
            call(pytest.approx(-7.0)),
            call(pytest.approx(0.0)),
        ],
        any_order=False,
    )
