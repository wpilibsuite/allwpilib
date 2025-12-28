// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.drivedistanceoffboard;

import org.wpilib.command2.Command;
import org.wpilib.command2.Commands;
import org.wpilib.command2.button.CommandGamepad;
import org.wpilib.driverstation.Gamepad;
import org.wpilib.examples.drivedistanceoffboard.Constants.OIConstants;
import org.wpilib.examples.drivedistanceoffboard.subsystems.DriveSubsystem;

/**
 * This class is where the bulk of the robot should be declared. Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
 * subsystems, commands, and button mappings) should be declared here.
 */
public class RobotContainer {
  // The robot's subsystems
  private final DriveSubsystem m_robotDrive = new DriveSubsystem();

  // Retained command references
  private final Command m_driveFullSpeed = Commands.runOnce(() -> m_robotDrive.setMaxOutput(1));
  private final Command m_driveHalfSpeed = Commands.runOnce(() -> m_robotDrive.setMaxOutput(0.5));

  // The driver's controller
  CommandGamepad m_driverController = new CommandGamepad(OIConstants.kDriverControllerPort);

  /** The container for the robot. Contains subsystems, OI devices, and commands. */
  public RobotContainer() {
    // Configure the button bindings
    configureButtonBindings();

    // Configure default commands
    // Set the default drive command to split-stick arcade drive
    m_robotDrive.setDefaultCommand(
        // A split-stick arcade command, with forward/backward controlled by the left
        // hand, and turning controlled by the right.
        Commands.run(
            () ->
                m_robotDrive.arcadeDrive(
                    -m_driverController.getLeftY(), -m_driverController.getRightX()),
            m_robotDrive));
  }

  /**
   * Use this method to define your button->command mappings. Buttons can be created by
   * instantiating a {@link org.wpilib.driverstation.GenericHID} or one of its subclasses ({@link
   * org.wpilib.driverstation.Joystick} or {@link Gamepad}), and then passing it to a {@link
   * org.wpilib.command2.button.JoystickButton}.
   */
  private void configureButtonBindings() {
    // Drive at half speed when the bumper is held
    m_driverController.rightShoulder().onTrue(m_driveHalfSpeed).onFalse(m_driveFullSpeed);

    // Drive forward by 3 meters when the 'A' button is pressed, with a timeout of 10 seconds
    m_driverController.southFace().onTrue(m_robotDrive.profiledDriveDistance(3).withTimeout(10));

    // Do the same thing as above when the 'B' button is pressed, but without resetting the encoders
    m_driverController
        .eastFace()
        .onTrue(m_robotDrive.dynamicProfiledDriveDistance(3).withTimeout(10));
  }

  /**
   * Use this to pass the autonomous command to the main {@link Robot} class.
   *
   * @return the command to run in autonomous
   */
  public Command getAutonomousCommand() {
    return Commands.none();
  }
}
