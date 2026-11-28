#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from typing import override

from commands2 import Command, CommandScheduler, TimedCommandRobot
from commands2.button import CommandXboxController
from commands2.sysid import SysIdRoutine

from constants import OIConstants
from subsystems.drive import Drive
from subsystems.shooter import Shooter


class MyRobot(TimedCommandRobot):
    """
    The methods in this class are called automatically corresponding to each mode,
    as described in the TimedRobot documentation.
    """

    def __init__(self) -> None:
        """This function is run when the robot is first started up and should be used for any
        initialization code.
        """
        super().__init__()
        # The robot's subsystems
        self.drive = Drive()
        self.shooter = Shooter()

        # The driver's controller
        self.controller = CommandXboxController(OIConstants.DRIVER_CONTROLLER_PORT)

        # Configure default commands and condition bindings on robot startup
        self.configure_bindings()

        self.autonomous_command = None

    def configure_bindings(self) -> None:
        """Use this method to define bindings between conditions and commands. These are useful for
        automating robot behaviors based on button and sensor input.

        Should be called in the robot class constructor.

        Event binding methods are available on the :class:`.Trigger` class.
        """

        # Control the drive with split-stick arcade controls
        self.drive.set_default_command(
            self.drive.arcade_drive_command(
                lambda: -self.controller.get_left_y(),
                lambda: -self.controller.get_right_x(),
            )
        )

        # Bind full set of SysId routine tests to buttons; a complete routine should run each of these
        # once.
        # Using bumpers as a modifier and combining it with the buttons so that we can have both sets
        # of bindings at once
        self.controller.a().and_(self.controller.right_bumper()).while_true(
            self.drive.sys_id_quasistatic(SysIdRoutine.Direction.FORWARD)
        )
        self.controller.b().and_(self.controller.right_bumper()).while_true(
            self.drive.sys_id_quasistatic(SysIdRoutine.Direction.REVERSE)
        )
        self.controller.x().and_(self.controller.right_bumper()).while_true(
            self.drive.sys_id_dynamic(SysIdRoutine.Direction.FORWARD)
        )
        self.controller.y().and_(self.controller.right_bumper()).while_true(
            self.drive.sys_id_dynamic(SysIdRoutine.Direction.REVERSE)
        )

        # Control the shooter wheel with the left trigger
        self.shooter.set_default_command(
            self.shooter.run_shooter(self.controller.get_left_trigger)
        )

        self.controller.a().and_(self.controller.left_bumper()).while_true(
            self.shooter.sys_id_quasistatic(SysIdRoutine.Direction.FORWARD)
        )
        self.controller.b().and_(self.controller.left_bumper()).while_true(
            self.shooter.sys_id_quasistatic(SysIdRoutine.Direction.REVERSE)
        )
        self.controller.x().and_(self.controller.left_bumper()).while_true(
            self.shooter.sys_id_dynamic(SysIdRoutine.Direction.FORWARD)
        )
        self.controller.y().and_(self.controller.left_bumper()).while_true(
            self.shooter.sys_id_dynamic(SysIdRoutine.Direction.REVERSE)
        )

    @override
    def disabled_start(self) -> None:
        """This function is called each time Disabled mode starts."""
        pass

    @override
    def disabled_periodic(self) -> None:
        pass

    @override
    def autonomous_start(self) -> None:
        # Do nothing
        self.autonomous_command = self.drive.run(lambda: None)
        CommandScheduler.get_instance().schedule(self.autonomous_command)

    @override
    def autonomous_periodic(self) -> None:
        """This function is called periodically during autonomous."""
        pass

    @override
    def teleop_start(self) -> None:
        # This makes sure that the autonomous stops running when
        # teleop starts running. If you want the autonomous to
        # continue until interrupted by another command, remove
        # this line or comment it out.
        if self.autonomous_command is not None:
            self.autonomous_command.cancel()

    @override
    def teleop_periodic(self) -> None:
        """This function is called periodically during teleoperated mode."""
        pass

    @override
    def utility_start(self) -> None:
        # Cancels all running commands at the start of utility mode.
        CommandScheduler.get_instance().cancel_all()

    @override
    def utility_periodic(self) -> None:
        """This function is called periodically during utility mode."""
        pass
