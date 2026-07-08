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
        self.onboardIO = xrp.XRPOnBoardIO()
        self.arm = Arm()

        # Assumes a gamepad plugged into channnel 0
        self.controller = wpilib.Joystick(0)

        # Create SmartDashboard chooser for autonomous routines
        self.chooser = wpilib.SendableChooser()

        self._configureButtonBindings()

    def _configureButtonBindings(self):
        """Use this method to define your button->command mappings. Buttons can be created by
        instantiating a :class:`.GenericHID` or one of its subclasses (:class`.Joystick or
        :class:`.XboxController`), and then passing it to a :class:`.JoystickButton`.
        """

        # Default command is arcade drive. This will run unless another command
        # is scheduled over it
        self.drivetrain.setDefaultCommand(self.getArcadeDriveCommand())

        # Example of how to use the onboard IO
        onboardButtonA = commands2.button.Trigger(self.onboardIO.getUserButtonPressed)
        onboardButtonA.onTrue(commands2.PrintCommand("USER Button Pressed")).onFalse(
            commands2.PrintCommand("USER Button Released")
        )

        joystickAButton = commands2.button.JoystickButton(self.controller, 1)
        joystickAButton.onTrue(
            commands2.InstantCommand(lambda: self.arm.setAngle(45.0), self.arm)
        )
        joystickAButton.onFalse(
            commands2.InstantCommand(lambda: self.arm.setAngle(0.0), self.arm)
        )

        joystickBButton = commands2.button.JoystickButton(self.controller, 2)
        joystickBButton.onTrue(
            commands2.InstantCommand(lambda: self.arm.setAngle(90.0), self.arm)
        )
        joystickBButton.onFalse(
            commands2.InstantCommand(lambda: self.arm.setAngle(0.0), self.arm)
        )

        # Setup SmartDashboard options
        self.chooser.setDefaultOption(
            "Auto Routine Distance", AutonomousDistance(self.drivetrain)
        )
        self.chooser.addOption("Auto Routine Time", AutonomousTime(self.drivetrain))
        wpilib.SmartDashboard.putData(self.chooser)

    def getAutonomousCommand(self) -> typing.Optional[commands2.Command]:
        return self.chooser.getSelected()

    def getArcadeDriveCommand(self) -> ArcadeDrive:
        """Use this to pass the teleop command to the main robot class.

        :returns: the command to run in teleop
        """
        return ArcadeDrive(
            self.drivetrain,
            lambda: -self.controller.getRawAxis(1),
            lambda: -self.controller.getRawAxis(2),
        )
