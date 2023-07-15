// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.frisbeebot;

import edu.wpi.first.wpilibj.examples.frisbeebot.Constants.AutoConstants;
import edu.wpi.first.wpilibj.examples.frisbeebot.Constants.OIConstants;
import edu.wpi.first.wpilibj.examples.frisbeebot.subsystems.DriveSubsystem;
import edu.wpi.first.wpilibj.examples.frisbeebot.subsystems.ShooterSubsystem;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.Commands;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;

/**
 * This class is where the bulk of the robot should be declared. Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
 * subsystems, commands, and button mappings) should be declared here.
 */
public class RobotContainer {
  // The robot's subsystems
  private final DriveSubsystem m_robotDrive = new DriveSubsystem();
  private final ShooterSubsystem m_shooter = new ShooterSubsystem();

  private final Command m_spinUpShooter = Commands.runOnce(m_shooter::enable, m_shooter);
  private final Command m_stopShooter = Commands.runOnce(m_shooter::disable, m_shooter);

  // An autonomous routine that shoots the loaded frisbees
  Command m_autonomousCommand =
      Commands.sequence(
              // Start the command by spinning up the shooter...
              Commands.runOnce(m_shooter::enable, m_shooter),
              // Wait until the shooter is at speed before feeding the frisbees
              Commands.waitUntil(m_shooter::atSetpoint),
              // Start running the feeder
              Commands.runOnce(m_shooter::runFeeder, m_shooter),
              // Shoot for the specified time
              Commands.waitSeconds(AutoConstants.kAutoShootTimeSeconds))
          // Add a timeout (will end the command if, for instance, the shooter
          // never gets up to speed)
          .withTimeout(AutoConstants.kAutoTimeoutSeconds)
          // When the command ends, turn off the shooter and the feeder
          .andThen(
              Commands.runOnce(
                  () -> {
                    m_shooter.disable();
                    m_shooter.stopFeeder();
                  }));

  // The driver's controller
  CommandXboxController m_driverController =
      new CommandXboxController(OIConstants.kDriverControllerPort);

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
   * Use this method to define your button->command mappings. Buttons can be created via the button
   * factories on {@link edu.wpi.first.wpilibj2.command.button.CommandGenericHID} or one of its
   * subclasses ({@link edu.wpi.first.wpilibj2.command.button.CommandJoystick} or {@link
   * CommandXboxController}).
   */
  private void configureButtonBindings() {
    // Configure your button bindings here

    // We can bind commands while retaining references to them in RobotContainer

    // Spin up the shooter when the 'A' button is pressed
    m_driverController.a().onTrue(m_spinUpShooter);

    // Turn off the shooter when the 'B' button is pressed
    m_driverController.b().onTrue(m_stopShooter);

    // We can also write them as temporary variables outside the bindings

    // Shoots if the shooter wheel has reached the target speed
    Command shoot =
        Commands.either(
            // Run the feeder
            Commands.runOnce(m_shooter::runFeeder, m_shooter),
            // Do nothing
            Commands.none(),
            // Determine which of the above to do based on whether the shooter has reached the
            // desired speed
            m_shooter::atSetpoint);

    Command stopFeeder = Commands.runOnce(m_shooter::stopFeeder, m_shooter);

    // Shoot when the 'X' button is pressed
    m_driverController.x().onTrue(shoot).onFalse(stopFeeder);

    // We can also define commands inline at the binding!

    // While holding the shoulder button, drive at half speed
    m_driverController
        .rightBumper()
        .onTrue(Commands.runOnce(() -> m_robotDrive.setMaxOutput(0.5)))
        .onFalse(Commands.runOnce(() -> m_robotDrive.setMaxOutput(1)));
  }

  /**
   * Use this to pass the autonomous command to the main {@link Robot} class.
   *
   * @return the command to run in autonomous
   */
  public Command getAutonomousCommand() {
    return m_autonomousCommand;
  }
}
