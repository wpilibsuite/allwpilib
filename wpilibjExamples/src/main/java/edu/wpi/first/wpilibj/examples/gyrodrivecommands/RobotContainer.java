// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gyrodrivecommands;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.wpilibj.PS4Controller;
import edu.wpi.first.wpilibj.examples.gyrodrivecommands.Constants.OIConstants;
import edu.wpi.first.wpilibj.examples.gyrodrivecommands.subsystems.Drive;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.Commands;
import edu.wpi.first.wpilibj2.command.InstantCommand;
import edu.wpi.first.wpilibj2.command.RunCommand;
import edu.wpi.first.wpilibj2.command.button.CommandPS4Controller;

/**
 * This class is where the bulk of the robot should be declared. Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
 * subsystems, commands, and button mappings) should be declared here.
 */
public class RobotContainer {
  // The robot's subsystems
  private final Drive m_drive = new Drive();

  // The driver's controller
  CommandPS4Controller m_driverController =
      new CommandPS4Controller(OIConstants.kDriverControllerPort);

  /** The container for the robot. Contains subsystems, OI devices, and commands. */
  public RobotContainer() {
    // Configure the button bindings
    configureButtonBindings();

    // Configure default commands
    // Set the default drive command to split-stick arcade drive
    m_drive.setDefaultCommand(
        // A split-stick arcade command, with forward/backward controlled by the left
        // hand, and turning controlled by the right.
        new RunCommand(
            () ->
                m_drive.arcadeDrive(
                    -m_driverController.getLeftY(), -m_driverController.getRightX()),
            m_drive));
  }

  /**
   * Use this method to define your button->command mappings. Buttons can be created by
   * instantiating a {@link edu.wpi.first.wpilibj.GenericHID} or one of its subclasses ({@link
   * edu.wpi.first.wpilibj.Joystick} or {@link PS4Controller}), and then passing it to a {@link
   * edu.wpi.first.wpilibj2.command.button.JoystickButton}.
   */
  private void configureButtonBindings() {
    // Drive at half speed when the right bumper is held
    m_driverController
        .R1()
        .onTrue(new InstantCommand(() -> m_drive.setMaxOutput(0.5)))
        .onFalse(new InstantCommand(() -> m_drive.setMaxOutput(1)));

    // Stabilize robot to drive straight with gyro when left bumper is held
    m_driverController.L1().whileTrue(m_drive.stabilize(() -> -m_driverController.getLeftY()));

    // Turn to 90 degrees when the 'X' button is pressed, with a 5 second timeout
    m_driverController
        .cross()
        .onTrue(m_drive.turnToAngle(Rotation2d.fromDegrees(90)).withTimeout(5));

    // Turn to -90 degrees with a profile when the Circle button is pressed, with a 5 second timeout
    m_driverController
        .circle()
        .onTrue(m_drive.turnToAngleProfiled(Rotation2d.fromDegrees(-90)).withTimeout(5));
  }

  /**
   * Use this to pass the autonomous command to the main {@link Robot} class.
   *
   * @return the command to run in autonomous
   */
  public Command getAutonomousCommand() {
    // no auto
    return Commands.none();
  }
}
