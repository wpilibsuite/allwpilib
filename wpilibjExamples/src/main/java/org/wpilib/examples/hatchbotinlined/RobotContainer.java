// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbotinlined;

import org.wpilib.command2.Command;
import org.wpilib.command2.Commands;
import org.wpilib.command2.button.CommandPS4Controller;
import org.wpilib.driverstation.PS4Controller;
import org.wpilib.examples.hatchbotinlined.Constants.OIConstants;
import org.wpilib.examples.hatchbotinlined.commands.Autos;
import org.wpilib.examples.hatchbotinlined.subsystems.DriveSubsystem;
import org.wpilib.examples.hatchbotinlined.subsystems.HatchSubsystem;
import org.wpilib.smartdashboard.SendableChooser;
import org.wpilib.smartdashboard.SmartDashboard;

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

  // Retained command handles

  // The autonomous routines
  // A simple auto routine that drives forward a specified distance, and then stops.
  private final Command m_simpleAuto = Autos.simpleAuto(m_robotDrive);
  // A complex auto routine that drives forward, drops a hatch, and then drives backward.
  private final Command m_complexAuto = Autos.complexAuto(m_robotDrive, m_hatchSubsystem);

  // A chooser for autonomous commands
  SendableChooser<Command> m_chooser = new SendableChooser<>();

  // The driver's controller
  CommandPS4Controller m_driverController =
      new CommandPS4Controller(OIConstants.kDriverControllerPort);

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

    // Add commands to the autonomous command chooser
    m_chooser.setDefaultOption("Simple Auto", m_simpleAuto);
    m_chooser.addOption("Complex Auto", m_complexAuto);

    // Put the chooser on the dashboard
    SmartDashboard.putData("Autonomous", m_chooser);

    // Put subsystems to dashboard.
    SmartDashboard.putData("Drivetrain", m_robotDrive);
    SmartDashboard.putData("HatchSubsystem", m_hatchSubsystem);
  }

  /**
   * Use this method to define your button->command mappings. Buttons can be created by
   * instantiating a {@link org.wpilib.driverstation.GenericHID} or one of its subclasses ({@link
   * org.wpilib.driverstation.Joystick} or {@link PS4Controller}), and then passing it to a {@link
   * org.wpilib.command2.button.JoystickButton}.
   */
  private void configureButtonBindings() {
    // Grab the hatch when the Circle button is pressed.
    m_driverController.circle().onTrue(m_hatchSubsystem.grabHatchCommand());
    // Release the hatch when the Square button is pressed.
    m_driverController.square().onTrue(m_hatchSubsystem.releaseHatchCommand());
    // While holding R1, drive at half speed
    m_driverController
        .R1()
        .onTrue(Commands.runOnce(() -> m_robotDrive.setMaxOutput(0.5)))
        .onFalse(Commands.runOnce(() -> m_robotDrive.setMaxOutput(1)));
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
