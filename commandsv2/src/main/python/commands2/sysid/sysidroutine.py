# validated: 2024-02-20 DV ee15cc172a5e sysid/SysIdRoutine.java
from dataclasses import dataclass, field
from enum import Enum

from wpilib.sysid import SysIdRoutineLog, State
from ..command import Command
from ..subsystem import Subsystem
from wpilib import Timer

from wpimath.units import seconds, volts

from typing import Callable, Optional

volts_per_second = float


class SysIdRoutine(SysIdRoutineLog):
    """A SysId characterization routine for a single mechanism. Mechanisms may have multiple motors.

    A single subsystem may have multiple mechanisms, but mechanisms should not share test
    routines. Each complete test of a mechanism should have its own SysIdRoutine instance, since the
    log name of the recorded data is determined by the mechanism name.

    The test state (e.g. "quasistatic-forward") is logged once per iteration during test
    execution, and once with state "none" when a test ends. Motor frames are logged every iteration
    during test execution.

    Timestamps are not coordinated across data, so motor frames and test state tags may be
    recorded on different log frames. Because frame alignment is not guaranteed, SysId parses the log
    by using the test state flag to determine the timestamp range for each section of the test, and
    then extracts the motor frames within the valid timestamp ranges. If a given test was run
    multiple times in a single logfile, the user will need to select which of the tests to use for
    the fit in the analysis tool.
    """

    @dataclass
    class Config:
        """Hardware-independent configuration for a SysId test routine.

        :param ramp_rate:    The voltage ramp rate used for quasistatic test routines. Defaults to 1 volt
                            per second if left null.
        :param step_voltage: The step voltage output used for dynamic test routines. Defaults to 7
                            volts if left null.
        :param timeout:     Safety timeout for the test routine commands. Defaults to 10 seconds if left
                            null.
        :param record_state: Optional handle for recording test state in a third-party logging
                            solution. If provided, the test routine state will be passed to this callback instead of
                            logged in WPILog.
        """

        ramp_rate: volts_per_second = 1.0
        step_voltage: volts = 7.0
        timeout: seconds = 10.0
        record_state: Optional[Callable[[State], None]] = None

    @dataclass
    class Mechanism:
        """A mechanism to be characterized by a SysId routine.

        Defines callbacks needed for the SysId test routine to control
        and record data from the mechanism.

        :param drive:     Sends the SysId-specified drive signal to the mechanism motors during test
                          routines.
        :param log:       Returns measured data of the mechanism motors during test routines. To return
                          data, call `motor(name)` on the supplied `SysIdRoutineLog` instance, and then
                          call one or more of the chainable logging handles (e.g. `voltage`) on the returned
                          `MotorLog`. Multiple motors can be logged in a single callback by calling `motor`
                          multiple times.
        :param subsystem: The subsystem containing the motor(s) that is (or are) being characterized.
                          Will be declared as a requirement for the returned test commands.
        :param name:      The name of the mechanism being tested. Will be appended to the log entry title
                          for the routine's test state, e.g. "sysid-test-state-mechanism". Defaults to the name of
                          the subsystem if left null.
        """

        drive: Callable[[volts], None]
        log: Callable[[SysIdRoutineLog], None]
        subsystem: Subsystem
        name: str = None  # type: ignore[assignment]

        def __post_init__(self):
            if self.name is None:
                self.name = self.subsystem.get_name()

    class Direction(Enum):
        """Motor direction for a SysId test."""

        FORWARD = 1
        REVERSE = -1

    def __init__(self, config: Config, mechanism: Mechanism):
        """Create a new SysId characterization routine.

        :param config:    Hardware-independent parameters for the SysId routine.
        :param mechanism: Hardware interface for the SysId routine.
        """
        super().__init__(mechanism.name)
        self.config = config
        self.mechanism = mechanism
        self.output_volts = 0.0
        self.log_state = config.record_state or self.record_state

    def quasistatic(self, direction: Direction) -> Command:
        """Returns a command to run a quasistatic test in the specified direction.

        The command will call the `drive` and `log` callbacks supplied at routine construction once
        per iteration. Upon command end or interruption, the `drive` callback is called with a value of
        0 volts.

        :param direction: The direction in which to run the test.

        :returns: A command to run the test.
        """

        timer = Timer()
        if direction == self.Direction.FORWARD:
            state = State.QUASISTATIC_FORWARD
        else:
            state = State.QUASISTATIC_REVERSE

        def execute():
            self.output_volts = direction.value * timer.get() * self.config.ramp_rate
            self.mechanism.drive(self.output_volts)
            self.mechanism.log(self)
            self.log_state(state)

        def end(interrupted: bool):
            self.mechanism.drive(0.0)
            self.log_state(State.NONE)
            timer.stop()

        return (
            self.mechanism.subsystem.run_once(timer.restart)
            .and_then(self.mechanism.subsystem.run(execute))
            .finally_do(end)
            .with_name(f"sysid-{state}-{self.mechanism.name}")
            .with_timeout(self.config.timeout)
        )

    def dynamic(self, direction: Direction) -> Command:
        """Returns a command to run a dynamic test in the specified direction.

        The command will call the `drive` and `log` callbacks supplied at routine construction once
        per iteration. Upon command end or interruption, the `drive` callback is called with a value of
        0 volts.

        :param direction: The direction in which to run the test.

        :returns: A command to run the test.
        """

        if direction == self.Direction.FORWARD:
            state = State.DYNAMIC_FORWARD
        else:
            state = State.DYNAMIC_REVERSE

        def command():
            self.output_volts = direction.value * self.config.step_voltage

        def execute():
            self.mechanism.drive(self.output_volts)
            self.mechanism.log(self)
            self.log_state(state)

        def end(interrupted: bool):
            self.mechanism.drive(0.0)
            self.log_state(State.NONE)

        return (
            self.mechanism.subsystem.run_once(command)
            .and_then(self.mechanism.subsystem.run(execute))
            .finally_do(end)
            .with_name(f"sysid-{state}-{self.mechanism.name}")
            .with_timeout(self.config.timeout)
        )
