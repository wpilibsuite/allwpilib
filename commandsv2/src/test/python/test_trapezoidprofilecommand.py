# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

from typing import TYPE_CHECKING, List, Tuple
import math

import wpimath.controller as controller
import wpimath.trajectory as trajectory
import wpimath.geometry as geometry
import wpimath.kinematics as kinematics
from wpimath.trajectory import TrapezoidProfile as DimensionlessProfile
from wpimath.trajectory import TrapezoidProfileRadians as RadiansProfile

from wpilib import Timer

from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest

import commands2


class TrapezoidProfileRadiansFixture:
    def __init__(self):
        constraints: RadiansProfile.Constraints = RadiansProfile.Constraints(
            3 * math.pi, math.pi
        )
        self._profile: RadiansProfile = RadiansProfile(constraints)
        self._goal_state = RadiansProfile.State(3, 0)

        self._state = self._profile.calculate(
            0, self._goal_state, RadiansProfile.State(0, 0)
        )

        self._timer = Timer()

    def profileOutput(self, state: RadiansProfile.State) -> None:
        self._state = state

    def currentState(self) -> RadiansProfile.State:
        return self._state

    def getGoal(self) -> RadiansProfile.State:
        return self._goal_state


@pytest.fixture()
def get_trapezoid_profile_radians() -> TrapezoidProfileRadiansFixture:
    return TrapezoidProfileRadiansFixture()


class TrapezoidProfileFixture:
    def __init__(self):
        constraints: DimensionlessProfile.Constraints = (
            DimensionlessProfile.Constraints(3 * math.pi, math.pi)
        )
        self._profile: DimensionlessProfile = DimensionlessProfile(constraints)
        self._goal_state = DimensionlessProfile.State(3, 0)

        self._state = self._profile.calculate(
            0, self._goal_state, DimensionlessProfile.State(0, 0)
        )

        self._timer = Timer()

    def profileOutput(self, state: DimensionlessProfile.State) -> None:
        self._state = state

    def currentState(self) -> DimensionlessProfile.State:
        return self._state

    def getGoal(self) -> DimensionlessProfile.State:
        return self._goal_state


@pytest.fixture()
def get_trapezoid_profile_dimensionless() -> TrapezoidProfileFixture:
    return TrapezoidProfileFixture()


def test_trapezoidProfileDimensionless(
    scheduler: commands2.CommandScheduler, get_trapezoid_profile_dimensionless
):
    with ManualSimTime() as sim:
        subsystem = commands2.Subsystem()

        fixture_data = get_trapezoid_profile_dimensionless

        command = commands2.TrapezoidProfileCommand(
            fixture_data._profile,
            fixture_data.profileOutput,
            fixture_data.getGoal,
            fixture_data.currentState,
            subsystem,
        )

        fixture_data._timer.restart()

        command.initialize()

        count = 0
        while not command.isFinished():
            command.execute()
            count += 1
            sim.step(0.005)

        fixture_data._timer.stop()
        command.end(True)


def test_trapezoidProfileRadians(
    scheduler: commands2.CommandScheduler, get_trapezoid_profile_radians
):
    with ManualSimTime() as sim:
        subsystem = commands2.Subsystem()

        fixture_data = get_trapezoid_profile_radians

        command = commands2.TrapezoidProfileCommand(
            fixture_data._profile,
            fixture_data.profileOutput,
            fixture_data.getGoal,
            fixture_data.currentState,
            subsystem,
        )

        fixture_data._timer.restart()

        command.initialize()

        count = 0
        while not command.isFinished():
            command.execute()
            count += 1
            sim.step(0.005)

        fixture_data._timer.stop()
        command.end(True)
