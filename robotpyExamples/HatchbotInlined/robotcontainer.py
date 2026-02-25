#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib

import commands2
import commands2.button
import commands2.cmd

import constants

import commands.autos

from subsystems.drivesubsystem import DriveSubsystem
from subsystems.hatchsubsystem import HatchSubsystem


class RobotContainer:
    """
    This class is where the bulk of the robot should be declared. Since Command-based is a
    "declarative" paradigm, very little robot logic should actually be handled in the :class:`.Robot`
    periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
    subsystems, commands, and button mappings) should be declared here.
    """

    def __init__(self) -> None:
        # The robot's subsystems
        self.driveSubsystem = DriveSubsystem()
        self.hatchSubsystem = HatchSubsystem()

        # Retained command handles

        # A simple auto routine that drives forward a specified distance, and then stops.
        self.simpleAuto = commands.autos.Autos.simpleAuto(self.driveSubsystem)

        # A complex auto routine that drives forward, drops a hatch, and then drives backward.
        self.complexAuto = commands.autos.Autos.complexAuto(
            self.driveSubsystem, self.hatchSubsystem
        )

        # The driver's controller
        self.driverController = commands2.button.CommandNiDsPS4Controller(
            constants.kDriverControllerPort
        )

        # Configure the button bindings
        self.configureButtonBindings()

        # Configure default commands
        # Set the default drive command to split-stick arcade drive
        self.driveSubsystem.setDefaultCommand(
            # A split-stick arcade command, with forward/backward controlled by the left
            # hand, and turning controlled by the right.
            commands2.cmd.run(
                lambda: self.driveSubsystem.arcadeDrive(
                    -self.driverController.getLeftY(),
                    -self.driverController.getRightX(),
                ),
                self.driveSubsystem,
            )
        )

        # Chooser
        self.chooser = wpilib.SendableChooser()

        # Add commands to the autonomous command chooser
        self.chooser.setDefaultOption("Simple Auto", self.simpleAuto)
        self.chooser.addOption("Complex Auto", self.complexAuto)

        # Put the chooser on the dashboard
        wpilib.SmartDashboard.putData("Autonomous", self.chooser)

    def configureButtonBindings(self):
        """
        Use this method to define your button->command mappings. Buttons can be created by
        instantiating a wpilib.GenericHID or one of its subclasses (Joystick or XboxController),
        and then passing it to a JoystickButton.
        """

        # Grab the hatch when the Circle button is pressed.
        self.driverController.circle().onTrue(self.hatchSubsystem.grabHatch())

        # Release the hatch when the Square button is pressed.
        self.driverController.square().onTrue(self.hatchSubsystem.releaseHatch())

        # While holding R1, drive at half speed
        self.driverController.R1().onTrue(
            commands2.cmd.runOnce(lambda: self.driveSubsystem.setMaxOutput(0.5))
        ).onFalse(commands2.cmd.runOnce(lambda: self.driveSubsystem.setMaxOutput(1)))

    def getAutonomousCommand(self) -> commands2.Command:
        return self.chooser.getSelected()
