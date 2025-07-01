#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib

import commands2
import commands2.button

import constants

from commands.complexauto import ComplexAuto
from commands.drivedistance import DriveDistance
from commands.defaultdrive import DefaultDrive
from commands.grabhatch import GrabHatch
from commands.halvedrivevelocity import HalveDriveVelocity
from commands.releasehatch import ReleaseHatch

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
        # The driver's controller
        # self.driver_controller = wpilib.NiDsXboxController(constants.DRIVER_CONTROLLER_PORT)
        self.driver_controller = wpilib.Joystick(constants.DRIVER_CONTROLLER_PORT)

        # The robot's subsystems
        self.drive = DriveSubsystem()
        self.hatch = HatchSubsystem()

        # Autonomous routines

        # A simple auto routine that drives forward a specified distance, and then stops.
        self.simple_auto = DriveDistance(
            constants.AUTO_DRIVE_DISTANCE_INCHES,
            constants.AUTO_DRIVE_VELOCITY,
            self.drive,
        )

        # A complex auto routine that drives forward, drops a hatch, and then drives backward.
        self.complex_auto = ComplexAuto(self.drive, self.hatch)

        # Chooser
        self.chooser = wpilib.Selectable()

        # Add commands to the autonomous command chooser
        self.chooser.add_default("Simple Auto", self.simple_auto)
        self.chooser.add("Complex Auto", self.complex_auto)

        # Put the chooser on the dashboard
        wpilib.Tunables.publish("Autonomous", self.chooser)

        self.configure_button_bindings()

        # set up default drive command
        self.drive.set_default_command(
            DefaultDrive(
                self.drive,
                lambda: -self.driver_controller.get_y(),
                lambda: self.driver_controller.get_x(),
            )
        )

    def configure_button_bindings(self):
        """
        Use this method to define your button->command mappings. Buttons can be created by
        instantiating a wpilib.GenericHID or one of its subclasses (Joystick or XboxController),
        and then passing it to a JoystickButton.
        """

        commands2.button.JoystickButton(self.driver_controller, 1).on_true(
            GrabHatch(self.hatch)
        )

        commands2.button.JoystickButton(self.driver_controller, 2).on_true(
            ReleaseHatch(self.hatch)
        )

        commands2.button.JoystickButton(self.driver_controller, 3).while_true(
            HalveDriveVelocity(self.drive)
        )

    def get_autonomous_command(self) -> commands2.Command:
        return self.chooser.get_selected()
