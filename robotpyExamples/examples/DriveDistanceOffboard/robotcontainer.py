#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2
import commands2.cmd
import commands2.button
import constants
import subsystems.drivesubsystem


class RobotContainer:
    """
    This class is where the bulk of the robot should be declared. Since Command-based is a
    "declarative" paradigm, very little robot logic should actually be handled in the :class:`.Robot`
    periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
    subsystems, commands, and button mappings) should be declared here.

    """

    def __init__(self):
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
        self.driver_controller = commands2.button.CommandNiDsXboxController(
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
        subclasses (commands2.button.CommandJoystick or command2.button.CommandNiDsXboxController).
        """

        # Configure your button bindings here

        # We can bind commands while retaining references to them in RobotContainer

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

    def get_autonomous_command(self) -> commands2.Command:
        """
        Use this to pass the autonomous command to the main :class:`.Robot` class.

        :returns: the command to run in autonomous
        """
        return commands2.cmd.none()
