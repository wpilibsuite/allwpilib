#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from commands2 import Command
from commands2.button import CommandNiDsXboxController
from commands2.sysid import SysIdRoutine

from subsystems.drive import Drive
from subsystems.shooter import Shooter

from constants import OIConstants


class SysIdRoutineBot:
    """This class is where the bulk of the robot should be declared. Since Command-based is a
    "declarative" paradigm, very little robot logic should actually be handled in the :class:`.Robot`
    periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
    subsystems, commands, and button mappings) should be declared here.
    """

    def __init__(self) -> None:
        # The robot's subsystems
        self.drive = Drive()
        self.shooter = Shooter()

        # The driver's controller
        self.controller = CommandNiDsXboxController(OIConstants.kDriverControllerPort)

    def configureBindings(self) -> None:
        """Use this method to define bindings between conditions and commands. These are useful for
        automating robot behaviors based on button and sensor input.

        Should be called in the robot class constructor.

        Event binding methods are available on the :class:`.Trigger` class.
        """

        # Control the drive with split-stick arcade controls
        self.drive.setDefaultCommand(
            self.drive.arcadeDriveCommand(
                lambda: -self.controller.getLeftY(),
                lambda: -self.controller.getRightX(),
            )
        )

        # Bind full set of SysId routine tests to buttons; a complete routine should run each of these
        # once.
        # Using bumpers as a modifier and combining it with the buttons so that we can have both sets
        # of bindings at once
        self.controller.a().and_(self.controller.rightBumper()).whileTrue(
            self.drive.sysIdQuasistatic(SysIdRoutine.Direction.kForward)
        )
        self.controller.b().and_(self.controller.rightBumper()).whileTrue(
            self.drive.sysIdQuasistatic(SysIdRoutine.Direction.kReverse)
        )
        self.controller.x().and_(self.controller.rightBumper()).whileTrue(
            self.drive.sysIdDynamic(SysIdRoutine.Direction.kForward)
        )
        self.controller.y().and_(self.controller.rightBumper()).whileTrue(
            self.drive.sysIdDynamic(SysIdRoutine.Direction.kReverse)
        )

        # Control the shooter wheel with the left trigger
        self.shooter.setDefaultCommand(
            self.shooter.runShooter(self.controller.getLeftTriggerAxis)
        )

        self.controller.a().and_(self.controller.leftBumper()).whileTrue(
            self.shooter.sysIdQuasistatic(SysIdRoutine.Direction.kForward)
        )
        self.controller.b().and_(self.controller.leftBumper()).whileTrue(
            self.shooter.sysIdQuasistatic(SysIdRoutine.Direction.kReverse)
        )
        self.controller.x().and_(self.controller.leftBumper()).whileTrue(
            self.shooter.sysIdDynamic(SysIdRoutine.Direction.kForward)
        )
        self.controller.y().and_(self.controller.leftBumper()).whileTrue(
            self.shooter.sysIdDynamic(SysIdRoutine.Direction.kReverse)
        )

    def getAutonomousCommand(self) -> Command:
        """Use this to define the command that runs during autonomous.

        Scheduled during :meth:`.Robot.autonomousInit`.
        """

        # Do nothing
        return self.drive.run(lambda: None)
