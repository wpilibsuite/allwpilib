#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import typing

import commands2
import commands2.button
import wpilib
import xrp

from commands.arcadedrive import ArcadeDrive
from commands.autonomousdistance import AutonomousDistance
from commands.autonomoustime import AutonomousTime

from subsystems.arm import Arm
from subsystems.drivetrain import Drivetrain


class RobotContainer:
    """
    This class is where the bulk of the robot should be declared. Since Command-based is a
    "declarative" paradigm, very little robot logic should actually be handled in the :class:`.Robot`
    periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
    subsystems, commands, and button mappings) should be declared here.
    """

    def __init__(self) -> None:
        # The robot's subsystems and commands are defined here...
        self.drivetrain = Drivetrain()
        self.onboard_io = xrp.XRPOnBoardIO()
        self.arm = Arm()

        # Assumes a gamepad plugged into channnel 0
        self.controller = wpilib.Joystick(0)

        # Create SmartDashboard chooser for autonomous routines
        self.chooser = wpilib.SendableChooser()

        self._configure_button_bindings()

    def _configure_button_bindings(self):
        """Use this method to define your button->command mappings. Buttons can be created by
        instantiating a :class:`.GenericHID` or one of its subclasses (:class`.Joystick or
        :class:`.XboxController`), and then passing it to a :class:`.JoystickButton`.
        """

        # Default command is arcade drive. This will run unless another command
        # is scheduled over it
        self.drivetrain.set_default_command(self.get_arcade_drive_command())

        # Example of how to use the onboard IO
        onboard_button_a = commands2.button.Trigger(
            self.onboard_io.get_user_button_pressed
        )
        onboard_button_a.on_true(
            commands2.PrintCommand("USER Button Pressed")
        ).on_false(commands2.PrintCommand("USER Button Released"))

        joystick_a_button = commands2.button.JoystickButton(self.controller, 1)
        joystick_a_button.on_true(
            commands2.InstantCommand(lambda: self.arm.set_angle(45.0), self.arm)
        )
        joystick_a_button.on_false(
            commands2.InstantCommand(lambda: self.arm.set_angle(0.0), self.arm)
        )

        joystick_b_button = commands2.button.JoystickButton(self.controller, 2)
        joystick_b_button.on_true(
            commands2.InstantCommand(lambda: self.arm.set_angle(90.0), self.arm)
        )
        joystick_b_button.on_false(
            commands2.InstantCommand(lambda: self.arm.set_angle(0.0), self.arm)
        )

        # Setup SmartDashboard options
        self.chooser.set_default_option(
            "Auto Routine Distance", AutonomousDistance(self.drivetrain)
        )
        self.chooser.add_option("Auto Routine Time", AutonomousTime(self.drivetrain))
        wpilib.SmartDashboard.put_data(self.chooser)

    def get_autonomous_command(self) -> typing.Optional[commands2.Command]:
        return self.chooser.get_selected()

    def get_arcade_drive_command(self) -> ArcadeDrive:
        """Use this to pass the teleop command to the main robot class.

        :returns: the command to run in teleop
        """
        return ArcadeDrive(
            self.drivetrain,
            lambda: -self.controller.get_raw_axis(1),
            lambda: -self.controller.get_raw_axis(2),
        )
