// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbotcmdv3;

import org.wpilib.command3.Command;
import org.wpilib.command3.button.CommandGamepad;
import org.wpilib.driverstation.Gamepad;
import org.wpilib.examples.hatchbotcmdv3.Constants.OIConstants;
import org.wpilib.examples.hatchbotcmdv3.commands.Autos;
import org.wpilib.examples.hatchbotcmdv3.subsystems.DriveSubsystem;
import org.wpilib.examples.hatchbotcmdv3.subsystems.HatchSubsystem;
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
        robotDrive
            .runRepeatedly(
                () ->
                    robotDrive.arcadeDrive(
                        -driverController.getLeftY(), -driverController.getRightX()))
            .withPriority(Command.LOWEST_PRIORITY)
            .named("Split-Stick Arcade Drive (Default Command)"));

    // Add commands to the autonomous command chooser
    chooser.setDefaultOption("Simple Auto", simpleAuto);
    chooser.addOption("Complex Auto", complexAuto);

    // Put the chooser on the dashboard
    SmartDashboard.putData("Autonomous", chooser);
  }

  /**
   * Use this method to define your button->command mappings. Buttons can be created by
   * instantiating a {@link org.wpilib.driverstation.GenericHID} or one of its subclasses ({@link
   * org.wpilib.driverstation.Joystick} or {@link Gamepad}), and then passing it to a {@link
   * org.wpilib.command3.button.JoystickButton}.
   */
  private void configureButtonBindings() {
    // Grab the hatch when the Circle button is pressed.
    driverController.faceRight().onTrue(hatchSubsystem.grabHatchCommand());
    // Release the hatch when the Square button is pressed.
    driverController.faceLeft().onTrue(hatchSubsystem.releaseHatchCommand());
    // While holding R1, drive at half speed
    driverController
        .rightBumper()
        .onTrue(
            Command.noRequirements(coro -> robotDrive.setMaxOutput(0.5)).named("Set half speed"))
        .onFalse(
            Command.noRequirements(coro -> robotDrive.setMaxOutput(1)).named("Set full speed"));
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
