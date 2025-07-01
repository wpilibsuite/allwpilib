#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import typing

import commands2
import commands2.button
import wpilib
import romi

from commands.arcadedrive import ArcadeDrive
from commands.autonomousdistance import AutonomousDistance
from commands.autonomoustime import AutonomousTime

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
        self.onboard_io = romi.OnBoardIO(
            romi.OnBoardIO.ChannelMode.INPUT, romi.OnBoardIO.ChannelMode.INPUT
        )

        # Assumes a gamepad plugged into channnel 0
        self.controller = wpilib.Joystick(0)

        # Create a tunable selector for autonomous routines.
        self.chooser = wpilib.Selectable()

        # NOTE: The I/O pin functionality of the 5 exposed I/O pins depends on the hardware "overlay"
        # that is specified when launching the wpilib-ws server on the Romi raspberry pi.
        # By default, the following are available (listed in order from inside of the board to outside):
        # - DIO 8 (mapped to Arduino pin 11, closest to the inside of the board)
        # - Analog In 0 (mapped to Analog Channel 6 / Arduino Pin 4)
        # - Analog In 1 (mapped to Analog Channel 2 / Arduino Pin 20)
        # - PWM 2 (mapped to Arduino Pin 21)
        # - PWM 3 (mapped to Arduino Pin 22)
        #
        # Your subsystem configuration should take the overlays into account

        self._configure_button_bindings()

    def _configure_button_bindings(self):
        """Use this method to define your button->command mappings. Buttons can be created by
        instantiating a :class:`.GenericHID` or one of its subclasses (:class`.Joystick or
        :class:`.XboxController`), and then passing it to a :class:`.JoystickButton`.
        """

        # Default command is arcade drive. This will run unless another command
        # is scheduler over it
        self.drivetrain.set_default_command(self.get_arcade_drive_command())

        # Example of how to use the onboard IO
        onboard_button_a = commands2.button.Trigger(
            self.onboard_io.get_button_a_pressed
        )
        onboard_button_a.on_true(commands2.PrintCommand("Button A Pressed")).on_false(
            commands2.PrintCommand("Button A Released")
        )

        # Set up autonomous options.
        self.chooser.add_default(
            "Auto Routine Distance", AutonomousDistance(self.drivetrain)
        )
        self.chooser.add("Auto Routine Time", AutonomousTime(self.drivetrain))
        wpilib.Tunables.publish("Autonomous", self.chooser)

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
