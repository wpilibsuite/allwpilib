// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.hatchbotinlined;

import static edu.wpi.first.wpilibj.PS4Controller.Button;

import edu.wpi.first.wpilibj.PS4Controller;
import edu.wpi.first.wpilibj.examples.hatchbotinlined.Constants.AutoConstants;
import edu.wpi.first.wpilibj.examples.hatchbotinlined.Constants.OIConstants;
import edu.wpi.first.wpilibj.examples.hatchbotinlined.commands.ComplexAutoCommand;
import edu.wpi.first.wpilibj.examples.hatchbotinlined.subsystems.DriveSubsystem;
import edu.wpi.first.wpilibj.examples.hatchbotinlined.subsystems.HatchSubsystem;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;
import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.FunctionalCommand;
import edu.wpi.first.wpilibj2.command.InstantCommand;
import edu.wpi.first.wpilibj2.command.RunCommand;
import edu.wpi.first.wpilibj2.command.button.JoystickButton;

/**
 * This class is where the bulk of the robot should be declared. Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
 * subsystems, commands, and button mappings) should be declared here.
 */
public class RobotContainer {
  // The robot's subsystems
  private final DriveSubsystem m_robotDrive = new DriveSubsystem();
  private final HatchSubsystem m_hatchSubsystem = new HatchSubsystem();

  // The autonomous routines

  // A simple auto routine that drives forward a specified distance, and then stops.
  private final Command m_simpleAuto =
      new FunctionalCommand(
          // Reset encoders on command start
          m_robotDrive::resetEncoders,
          // Drive forward while the command is executing
          () -> m_robotDrive.arcadeDrive(AutoConstants.kAutoDriveSpeed, 0),
          // Stop driving at the end of the command
          interrupt -> m_robotDrive.arcadeDrive(0, 0),
          // End the command when the robot's driven distance exceeds the desired value
          () -> m_robotDrive.getAverageEncoderDistance() >= AutoConstants.kAutoDriveDistanceInches,
          // Require the drive subsystem
          m_robotDrive);

  // A complex auto routine that drives forward, drops a hatch, and then drives backward.
  private final Command m_complexAuto = new ComplexAutoCommand(m_robotDrive, m_hatchSubsystem);

  // A chooser for autonomous commands
  SendableChooser<Command> m_chooser = new SendableChooser<>();

  // The driver's controller
  PS4Controller m_driverController = new PS4Controller(OIConstants.kDriverControllerPort);

  /** The container for the robot. Contains subsystems, OI devices, and commands. */
  public RobotContainer() {
    // Configure the button bindings
    configureButtonBindings();

    // Configure default commands
    // Set the default drive command to split-stick arcade drive
    m_robotDrive.setDefaultCommand(
        // A split-stick arcade command, with forward/backward controlled by the left
        // hand, and turning controlled by the right.
        new RunCommand(
            () ->
                m_robotDrive.arcadeDrive(
                    m_driverController.getLeftY(), m_driverController.getRightX()),
            m_robotDrive));

    // Add commands to the autonomous command chooser
    m_chooser.setDefaultOption("Simple Auto", m_simpleAuto);
    m_chooser.addOption("Complex Auto", m_complexAuto);

    // Put the chooser on the dashboard
    Shuffleboard.getTab("Autonomous").add(m_chooser);
  }

  /**
   * Use this method to define your button->command mappings. Buttons can be created by
   * instantiating a {@link edu.wpi.first.wpilibj.GenericHID} or one of its subclasses ({@link
   * edu.wpi.first.wpilibj.Joystick} or {@link PS4Controller}), and then passing it to a {@link
   * edu.wpi.first.wpilibj2.command.button.JoystickButton}.
   */
  private void configureButtonBindings() {
    // Grab the hatch when the Circle button is pressed.
    new JoystickButton(m_driverController, Button.kCircle.value)
        .whenPressed(new InstantCommand(m_hatchSubsystem::grabHatch, m_hatchSubsystem));
    // Release the hatch when the Square button is pressed.
    new JoystickButton(m_driverController, Button.kSquare.value)
        .whenPressed(new InstantCommand(m_hatchSubsystem::releaseHatch, m_hatchSubsystem));
    // While holding R1, drive at half speed
    new JoystickButton(m_driverController, Button.kR1.value)
        .whenPressed(() -> m_robotDrive.setMaxOutput(0.5))
        .whenReleased(() -> m_robotDrive.setMaxOutput(1));
  }

  /**
   * Use this to pass the autonomous command to the main {@link Robot} class.
   *
   * @return the command to run in autonomous
   */
  public Command getAutonomousCommand() {
    return m_chooser.getSelected();
  }
}
