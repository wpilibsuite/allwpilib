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
        self.driverController = CommandNiDsXboxController(
            OIConstants.kDriverControllerPort
        )

    def configureBindings(self) -> None:
        """Use this method to define bindings between conditions and commands. These are useful for
        automating robot behaviors based on button and sensor input.

        Should be called in the robot class constructor.

        Event binding methods are available on the :class:`.Trigger` class.
        """
        # Automatically run the storage motor whenever the ball storage is not full,
        # and turn it off whenever it fills. Uses subsystem-hosted trigger to
        # improve readability and make inter-subsystem communication easier.
        self.storage.hasCargo.whileFalse(self.storage.runCommand())

        # Automatically disable and retract the intake whenever the ball storage is full.
        self.storage.hasCargo.onTrue(self.intake.retractCommand())

        # Control the drive with split-stick arcade controls
        self.drive.setDefaultCommand(
            self.drive.arcadeDriveCommand(
                lambda: -self.driverController.getLeftY(),
                lambda: -self.driverController.getRightX(),
            )
        )

        # Deploy the intake with the X button
        self.driverController.x().onTrue(self.intake.intakeCommand())
        # Retract the intake with the Y button
        self.driverController.y().onTrue(self.intake.retractCommand())

        # Fire the shooter with the A button
        self.driverController.a().onTrue(
            commands2.cmd.parallel(
                self.shooter.shootCommand(ShooterConstants.kShooterTargetRPS),
                self.storage.runCommand(),
            ).withName("Shoot")
        )

        # Toggle compressor with the Start button
        self.driverController.start().toggleOnTrue(
            self.pneumatics.disableCompressorCommand()
        )

    def getAutonomousCommand(self) -> commands2.Command:
        """Use this to define the command that runs during autonomous.

        Scheduled during :meth:`.Robot.autonomousInit`.
        """
        # Drive forward for 2 meters at half speed with a 3 second timeout
        return self.drive.driveDistanceCommand(
            AutoConstants.kDriveDistance, AutoConstants.kDriveSpeed
        ).withTimeout(AutoConstants.kTimeout)
