from typing import TYPE_CHECKING

from util import *  # type: ignore
import commands2

import wpimath

if TYPE_CHECKING:
    from .util import *

import pytest


def test_pidCommandSupplier(scheduler: commands2.CommandScheduler):
    with ManualSimTime() as sim:
        output_float = OOFloat(0.0)
        measurement_source = OOFloat(5.0)
        setpoint_source = OOFloat(2.0)
        pid_controller = wpimath.PIDController(0.1, 0.01, 0.001)
        system = commands2.Subsystem()
        pidCommand = commands2.PIDCommand(
            pid_controller,
            measurement_source,
            setpoint_source,
            output_float.set,
            system,
        )
        start_spying_on(pidCommand)
        scheduler.schedule(pidCommand)
        scheduler.run()
        sim.step(1)
        scheduler.run()

        assert scheduler.isScheduled(pidCommand)

        assert not pidCommand._controller.atSetpoint()

        # Tell the pid command we're at our setpoint through the controller
        measurement_source.set(setpoint_source())

        sim.step(2)

        scheduler.run()

        # Should be measuring error of 0 now
        assert pidCommand._controller.atSetpoint()


def test_pidCommandScalar(scheduler: commands2.CommandScheduler):
    with ManualSimTime() as sim:
        output_float = OOFloat(0.0)
        measurement_source = OOFloat(5.0)
        setpoint_source = 2.0
        pid_controller = wpimath.PIDController(0.1, 0.01, 0.001)
        system = commands2.Subsystem()
        pidCommand = commands2.PIDCommand(
            pid_controller,
            measurement_source,
            setpoint_source,
            output_float.set,
            system,
        )
        start_spying_on(pidCommand)
        scheduler.schedule(pidCommand)
        scheduler.run()
        sim.step(1)
        scheduler.run()

        assert scheduler.isScheduled(pidCommand)

        assert not pidCommand._controller.atSetpoint()

        # Tell the pid command we're at our setpoint through the controller
        measurement_source.set(setpoint_source)

        sim.step(2)

        scheduler.run()

        # Should be measuring error of 0 now
        assert pidCommand._controller.atSetpoint()


def test_withTimeout(scheduler: commands2.CommandScheduler):
    with ManualSimTime() as sim:
        output_float = OOFloat(0.0)
        measurement_source = OOFloat(5.0)
        setpoint_source = OOFloat(2.0)
        pid_controller = wpimath.PIDController(0.1, 0.01, 0.001)
        system = commands2.Subsystem()
        command1 = commands2.PIDCommand(
            pid_controller,
            measurement_source,
            setpoint_source,
            output_float.set,
            system,
        )
        start_spying_on(command1)

        timeout = command1.withTimeout(2)

        scheduler.schedule(timeout)
        scheduler.run()

        verify(command1).initialize()
        verify(command1).execute()
        assert not scheduler.isScheduled(command1)
        assert scheduler.isScheduled(timeout)

        sim.step(3)
        scheduler.run()

        verify(command1).end(True)
        verify(command1, never()).end(False)
        assert not scheduler.isScheduled(timeout)
