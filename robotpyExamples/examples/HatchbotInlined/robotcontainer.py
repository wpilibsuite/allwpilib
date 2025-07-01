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
        self.drive_subsystem = DriveSubsystem()
        self.hatch_subsystem = HatchSubsystem()

        # Retained command handles

        # A simple auto routine that drives forward a specified distance, and then stops.
        self.simple_auto = commands.autos.Autos.simple_auto(self.drive_subsystem)

        # A complex auto routine that drives forward, drops a hatch, and then drives backward.
        self.complex_auto = commands.autos.Autos.complex_auto(
            self.drive_subsystem, self.hatch_subsystem
        )

        # The driver's controller
        self.driver_controller = commands2.button.CommandNiDsPS4Controller(
            constants.DRIVER_CONTROLLER_PORT
        )

        # Configure the button bindings
        self.configure_button_bindings()

        # Configure default commands
        # Set the default drive command to split-stick arcade drive
        self.drive_subsystem.set_default_command(
            # A split-stick arcade command, with forward/backward controlled by the left
            # hand, and turning controlled by the right.
            commands2.cmd.run(
                lambda: self.drive_subsystem.arcade_drive(
                    -self.driver_controller.get_left_y(),
                    -self.driver_controller.get_right_x(),
                ),
                self.drive_subsystem,
            )
        )

        # Chooser
        self.chooser = wpilib.Selectable()

        # Add commands to the autonomous command chooser
        self.chooser.add_default("Simple Auto", self.simple_auto)
        self.chooser.add("Complex Auto", self.complex_auto)

        # Put the chooser on the dashboard
        wpilib.Tunables.publish("Autonomous", self.chooser)

    def configure_button_bindings(self):
        """
        Use this method to define your button->command mappings. Buttons can be created by
        instantiating a wpilib.GenericHID or one of its subclasses (Joystick or XboxController),
        and then passing it to a JoystickButton.
        """

        # Grab the hatch when the Circle button is pressed.
        self.driver_controller.circle().on_true(self.hatch_subsystem.grab_hatch())

        # Release the hatch when the Square button is pressed.
        self.driver_controller.square().on_true(self.hatch_subsystem.release_hatch())

        # While holding R1, drive at half velocity
        self.driver_controller.r1().on_true(
            commands2.cmd.run_once(lambda: self.drive_subsystem.set_max_output(0.5))
        ).on_false(
            commands2.cmd.run_once(lambda: self.drive_subsystem.set_max_output(1))
        )

    def get_autonomous_command(self) -> commands2.Command:
        return self.chooser.get_selected()
