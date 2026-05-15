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
        self.robotDrive = subsystems.drivesubsystem.DriveSubsystem()

        # Retained command references
        self.driveFullVelocity = commands2.cmd.runOnce(
            lambda: self.robotDrive.setMaxOutput(1), self.robotDrive
        )
        self.driveHalfVelocity = commands2.cmd.runOnce(
            lambda: self.robotDrive.setMaxOutput(0.5), self.robotDrive
        )

        # The driver's controller
        self.driverController = commands2.button.CommandNiDsXboxController(
            constants.OIConstants.kDriverControllerPort
        )

        # Configure the button bindings
        self.configureButtonBindings()

        # Configure default commands
        # Set the default drive command to split-stick arcade drive
        self.robotDrive.setDefaultCommand(
            # A split-stick arcade command, with forward/backward controlled by the left
            # hand, and turning controlled by the right.
            commands2.cmd.run(
                lambda: self.robotDrive.arcadeDrive(
                    -self.driverController.getLeftY(),
                    -self.driverController.getRightX(),
                ),
                self.robotDrive,
            )
        )

    def configureButtonBindings(self):
        """
        Use this method to define your button->command mappings. Buttons can be created via the button
        factories on commands2.button.CommandGenericHID or one of its
        subclasses (commands2.button.CommandJoystick or command2.button.CommandNiDsXboxController).
        """

        # Configure your button bindings here

        # We can bind commands while retaining references to them in RobotContainer

        # Drive at half velocity when the bumper is held
        self.driverController.rightBumper().onTrue(self.driveHalfVelocity).onFalse(
            self.driveFullVelocity
        )

        # Drive forward by 3 meters when the 'A' button is pressed, with a timeout of 10 seconds
        self.driverController.a().onTrue(
            self.robotDrive.profiledDriveDistance(3).withTimeout(10)
        )

        # Do the same thing as above when the 'B' button is pressed, but defined inline
        self.driverController.b().onTrue(
            self.robotDrive.dynamicProfiledDriveDistance(3).withTimeout(10)
        )

    def getAutonomousCommand(self) -> commands2.Command:
        """
        Use this to pass the autonomous command to the main :class:`.Robot` class.

        :returns: the command to run in autonomous
        """
        return commands2.cmd.none()
