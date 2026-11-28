#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from typing import override

import commands2
import commands2.cmd
import commands2.button
import constants
import subsystems.drivesubsystem

"""
The VM is configured to automatically run this class, and to call the functions corresponding to
each mode, as described in the TimedRobot documentation. If you change the name of this class or
the package after creating this project, you must also update the build.gradle file in the
project.
"""


class MyRobot(commands2.TimedCommandRobot):
    """
    Command v2 robots are encouraged to inherit from TimedCommandRobot, which
    has an implementation of robot_periodic which runs the scheduler for you
    """

    def __init__(self) -> None:
        """
        This function is run when the robot is first started up and should be used for any
        initialization code.
        """
        super().__init__()
        self.autonomous_command: commands2.Command | None = None

        # The robot's subsystems
        self.robot_drive = subsystems.drivesubsystem.DriveSubsystem()

        # Retained command references
        self.drive_full_velocity = commands2.cmd.run_once(
            lambda: self.robot_drive.set_max_output(1), self.robot_drive
        )
        self.drive_half_velocity = commands2.cmd.run_once(
            lambda: self.robot_drive.set_max_output(0.5), self.robot_drive
        )

        # The driver's controller
        self.driver_controller = commands2.button.CommandXboxController(
            constants.OIConstants.DRIVER_CONTROLLER_PORT
        )

        # Configure the button bindings
        self.configure_button_bindings()

        # Configure default commands
        # Set the default drive command to split-stick arcade drive
        self.robot_drive.set_default_command(
            # A split-stick arcade command, with forward/backward controlled by the left
            # hand, and turning controlled by the right.
            commands2.cmd.run(
                lambda: self.robot_drive.arcade_drive(
                    -self.driver_controller.get_left_y(),
                    -self.driver_controller.get_right_x(),
                ),
                self.robot_drive,
            )
        )

    def configure_button_bindings(self):
        """
        Use this method to define your button->command mappings. Buttons can be created via the button
        factories on commands2.button.CommandGenericHID or one of its
        subclasses (commands2.button.CommandJoystick or command2.button.CommandXboxController).
        """

        # Configure your button bindings here

        # We can bind commands while retaining references to them in MyRobot

        # Drive at half velocity when the bumper is held
        self.driver_controller.right_bumper().on_true(
            self.drive_half_velocity
        ).on_false(self.drive_full_velocity)

        # Drive forward by 3 meters when the 'A' button is pressed, with a timeout of 10 seconds
        self.driver_controller.a().on_true(
            self.robot_drive.profiled_drive_distance(3).with_timeout(10)
        )

        # Do the same thing as above when the 'B' button is pressed, but defined inline
        self.driver_controller.b().on_true(
            self.robot_drive.dynamic_profiled_drive_distance(3).with_timeout(10)
        )

    @override
    def disabled_start(self) -> None:
        """This function is called each time Disabled mode starts."""

    @override
    def disabled_periodic(self) -> None:
        """This function is called periodically when disabled"""

    @override
    def autonomous_start(self) -> None:
        """This autonomous runs your autonomous command."""
        self.autonomous_command = commands2.cmd.none()

        # schedule the autonomous command (example)
        self.autonomous_command.schedule()

    @override
    def autonomous_periodic(self) -> None:
        """This function is called periodically during autonomous"""

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
        """This function is called periodically during teleoperated mode"""

    @override
    def utility_start(self) -> None:
        # Cancels all running commands at the start of utility mode
        commands2.CommandScheduler.get_instance().cancel_all()
