"""
The primary purpose of these tests is to run through your code
and make sure that it doesn't crash. If you actually want to test
your code, you need to write your own custom tests to tease out
the edge cases.

To use these, add the following to a python file in your tests directory::

    from wpilib.testing.robot_tests import *

In addition to the standard autonomous/teleop tests, ``test_opmode`` runs each
published opmode (including utility modes) as a separate pytest parameter. Each
case gets a fresh robot and runs disabled, enabled for 15 simulated seconds, and
disabled again. Robots without published opmodes skip this test.

Discovery starts a separate robot during collection and reads the opmodes
published by startup, without constructing or enabling those opmodes. Modes
registered later during operation are not included. This also applies when
using the ``opmode`` fixture in custom tests.

"""

import pytest

from . import OpMode
from .controller import RobotTestController


def test_opmode(control: RobotTestController, opmode: OpMode):
    """Runs each published opmode through disabled, enabled, and disabled."""
    with control.run_robot():
        control.step_timing(seconds=0.5, opmode=opmode, enabled=False)
        control.step_timing(seconds=15, opmode=opmode, enabled=True)
        control.step_timing(seconds=0.5, opmode=opmode, enabled=False)


def test_autonomous(control: RobotTestController):
    """Runs autonomous mode by itself"""

    with control.run_robot():
        # Run disabled for a short period
        control.step_timing(seconds=0.5, autonomous=True, enabled=False)

        # Run enabled for 15 seconds
        control.step_timing(seconds=15, autonomous=True, enabled=True)

        # Disabled for another short period
        control.step_timing(seconds=0.5, autonomous=True, enabled=False)


@pytest.mark.filterwarnings("ignore")
def test_disabled(control: RobotTestController, robot):
    """Runs disabled mode by itself"""

    with control.run_robot():
        # Run disabled + autonomous for a short period
        control.step_timing(seconds=5, autonomous=True, enabled=False)

        # Run disabled + !autonomous for a short period
        control.step_timing(seconds=5, autonomous=False, enabled=False)


@pytest.mark.filterwarnings("ignore")
def test_operator_control(control: RobotTestController):
    """Runs operator control mode by itself"""

    with control.run_robot():
        # Run disabled for a short period
        control.step_timing(seconds=0.5, autonomous=False, enabled=False)

        # Run enabled for 15 seconds
        control.step_timing(seconds=15, autonomous=False, enabled=True)

        # Disabled for another short period
        control.step_timing(seconds=0.5, autonomous=False, enabled=False)


@pytest.mark.filterwarnings("ignore")
def test_practice(control: RobotTestController):
    """Runs through the entire span of a practice match"""

    with control.run_robot():
        # Run disabled for a short period
        control.step_timing(seconds=0.5, autonomous=True, enabled=False)

        # Run autonomous + enabled for 15 seconds
        control.step_timing(seconds=15, autonomous=True, enabled=True)

        # Disabled for another short period
        control.step_timing(seconds=0.5, autonomous=False, enabled=False)

        # Run teleop + enabled for 2 minutes
        control.step_timing(seconds=120, autonomous=False, enabled=True)
