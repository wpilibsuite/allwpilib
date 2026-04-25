// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbotinlined;

import org.wpilib.command2.Command;
import org.wpilib.command2.Commands;
import org.wpilib.command2.button.CommandGamepad;
import org.wpilib.driverstation.Gamepad;
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
  private final DriveSubsystem robotDrive = new DriveSubsystem();
  private final HatchSubsystem hatchSubsystem = new HatchSubsystem();

  // Retained command handles

  // The autonomous routines
  // A simple auto routine that drives forward a specified distance, and then stops.
  private final Command simpleAuto = Autos.simpleAuto(robotDrive);
  // A complex auto routine that drives forward, drops a hatch, and then drives backward.
  private final Command complexAuto = Autos.complexAuto(robotDrive, hatchSubsystem);

  // A chooser for autonomous commands
  SendableChooser<Command> chooser = new SendableChooser<>();

  // The driver's controller
  CommandGamepad driverController = new CommandGamepad(OIConstants.kDriverControllerPort);

  /** The container for the robot. Contains subsystems, OI devices, and commands. */
  public RobotContainer() {
    // Configure the button bindings
    configureButtonBindings();

    // Configure default commands
    // Set the default drive command to split-stick arcade drive
    robotDrive.setDefaultCommand(
        // A split-stick arcade command, with forward/backward controlled by the left
        // hand, and turning controlled by the right.
        Commands.run(
            () ->
                robotDrive.arcadeDrive(-driverController.getLeftY(), -driverController.getRightX()),
            robotDrive));

    // Add commands to the autonomous command chooser
    chooser.setDefaultOption("Simple Auto", simpleAuto);
    chooser.addOption("Complex Auto", complexAuto);

    // Put the chooser on the dashboard
    SmartDashboard.putData("Autonomous", chooser);

    // Put subsystems to dashboard.
    SmartDashboard.putData("Drivetrain", robotDrive);
    SmartDashboard.putData("HatchSubsystem", hatchSubsystem);
  }

  /**
   * Use this method to define your button->command mappings. Buttons can be created by
   * instantiating a {@link org.wpilib.driverstation.GenericHID} or one of its subclasses ({@link
   * org.wpilib.driverstation.Joystick} or {@link Gamepad}), and then passing it to a {@link
   * org.wpilib.command2.button.JoystickButton}.
   */
  private void configureButtonBindings() {
    // Grab the hatch when the east face button is pressed.
    driverController.eastFace().onTrue(hatchSubsystem.grabHatchCommand());
    // Release the hatch when the west face button is pressed.
    driverController.westFace().onTrue(hatchSubsystem.releaseHatchCommand());
    // While holding right bumper, drive at half velocity
    driverController
        .rightBumper()
        .onTrue(Commands.runOnce(() -> robotDrive.setMaxOutput(0.5)))
        .onFalse(Commands.runOnce(() -> robotDrive.setMaxOutput(1)));
  }

  /**
   * Use this to pass the autonomous command to the main {@link Robot} class.
   *
   * @return the command to run in autonomous
   */
  public Command getAutonomousCommand() {
    return chooser.getSelected();
  }
}
