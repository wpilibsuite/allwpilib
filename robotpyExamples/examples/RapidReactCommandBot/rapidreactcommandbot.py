#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2
from commands2.button import CommandNiDsXboxController, Trigger

from constants import AutoConstants, OIConstants, ShooterConstants
from subsystems.drive import Drive
from subsystems.intake import Intake
from subsystems.pneumatics import Pneumatics
from subsystems.shooter import Shooter
from subsystems.storage import Storage


class RapidReactCommandBot:
    """This class is where the bulk of the robot should be declared. Since Command-based is a
    "declarative" paradigm, very little robot logic should actually be handled in the
    :class:`.Robot` periodic methods (other than the scheduler calls). Instead, the structure of
    the robot (including subsystems, commands, and button mappings) should be declared here.
    """

    def __init__(self) -> None:
        # The robot's subsystems
        self.drive = Drive()
        self.intake = Intake()
        self.storage = Storage()
        self.shooter = Shooter()
        self.pneumatics = Pneumatics()

        # The driver's controller
        self.driver_controller = CommandNiDsXboxController(
            OIConstants.DRIVER_CONTROLLER_PORT
        )

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
        self.driver_controller.start().toggle_on_true(
            self.pneumatics.disable_compressor_command()
        )

    def get_autonomous_command(self) -> commands2.Command:
        """Use this to define the command that runs during autonomous.

        Scheduled during :meth:`.Robot.autonomous_init`.
        """
        # Drive forward for 2 meters at half velocity with a 3 second timeout
        return self.drive.drive_distance_command(
            AutoConstants.DRIVE_DISTANCE, AutoConstants.DRIVE_VELOCITY
        ).with_timeout(AutoConstants.TIMEOUT)
