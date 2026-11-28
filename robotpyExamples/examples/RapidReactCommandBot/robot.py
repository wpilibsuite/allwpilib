#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from typing import override

import commands2
from commands2.button import CommandXboxController
import wpilib

from constants import AutoConstants, OIConstants, ShooterConstants
from subsystems.drive import Drive
from subsystems.intake import Intake
from subsystems.pneumatics import Pneumatics
from subsystems.shooter import Shooter
from subsystems.storage import Storage


class MyRobot(commands2.TimedCommandRobot):
    """The methods in this class are called automatically corresponding to each mode, as
    described in the TimedRobot documentation. If you change the name of this class or the
    package after creating this project, you must also update the Main.java file in the
    project.
    """

    def __init__(self) -> None:
        """This function is run when the robot is first started up and should be used for any
        initialization code.
        """
        super().__init__()
        self.autonomous_command = None
        # The robot's subsystems
        self.drive = Drive()
        self.intake = Intake()
        self.storage = Storage()
        self.shooter = Shooter()
        self.pneumatics = Pneumatics()

        # The driver's controller
        self.driver_controller = CommandXboxController(
            OIConstants.DRIVER_CONTROLLER_PORT
        )

        # Configure default commands and condition bindings on robot startup
        self.configure_bindings()

        # Initialize data logging.
        wpilib.DataLogManager.start()

    def configure_bindings(self) -> None:
        """Use this method to define bindings between conditions and commands. These are useful for
        automating robot behaviors based on button and sensor input.

        Should be called in the robot class constructor.

        Event binding methods are available on the :class:`.Trigger` class.
        """
        # Automatically run the storage motor whenever the ball storage is not full,
        # and turn it off whenever it fills. Uses subsystem-hosted trigger to
        # improve readability and make inter-subsystem communication easier.
        self.storage.has_cargo.while_false(self.storage.run_command())

        # Automatically disable and retract the intake whenever the ball storage is full.
        self.storage.has_cargo.on_true(self.intake.retract_command())

        # Control the drive with split-stick arcade controls
        self.drive.set_default_command(
            self.drive.arcade_drive_command(
                lambda: -self.driver_controller.get_left_y(),
                lambda: -self.driver_controller.get_right_x(),
            )
        )

        # Deploy the intake with the X button
        self.driver_controller.x().on_true(self.intake.intake_command())
        # Retract the intake with the Y button
        self.driver_controller.y().on_true(self.intake.retract_command())

        # Fire the shooter with the A button
        self.driver_controller.a().on_true(
            commands2.cmd.parallel(
                self.shooter.shoot_command(ShooterConstants.SHOOTER_TARGET_RPS),
                self.storage.run_command(),
            ).with_name("Shoot")
        )

        # Toggle compressor with the Start button
        self.driver_controller.menu().toggle_on_true(
            self.pneumatics.disable_compressor_command()
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
        # Drive forward for 2 meters at half velocity with a 3 second timeout
        self.autonomous_command = self.drive.drive_distance_command(
            AutoConstants.DRIVE_DISTANCE, AutoConstants.DRIVE_VELOCITY
        ).with_timeout(AutoConstants.TIMEOUT)
        self.autonomous_command.schedule()

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
        commands2.CommandScheduler.get_instance().cancel_all()

    @override
    def utility_periodic(self) -> None:
        """This function is called periodically during utility mode."""
        pass
