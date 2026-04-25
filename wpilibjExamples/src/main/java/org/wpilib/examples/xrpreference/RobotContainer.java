// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.xrpreference;

import org.wpilib.command2.Command;
import org.wpilib.command2.InstantCommand;
import org.wpilib.command2.PrintCommand;
import org.wpilib.command2.button.JoystickButton;
import org.wpilib.command2.button.Trigger;
import org.wpilib.driverstation.Gamepad;
import org.wpilib.driverstation.GenericHID;
import org.wpilib.driverstation.Joystick;
import org.wpilib.examples.xrpreference.commands.ArcadeDrive;
import org.wpilib.examples.xrpreference.commands.AutonomousDistance;
import org.wpilib.examples.xrpreference.commands.AutonomousTime;
import org.wpilib.examples.xrpreference.subsystems.Arm;
import org.wpilib.examples.xrpreference.subsystems.Drivetrain;
import org.wpilib.smartdashboard.SendableChooser;
import org.wpilib.smartdashboard.SmartDashboard;
import org.wpilib.xrp.XRPOnBoardIO;

/**
 * This class is where the bulk of the robot should be declared. Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
 * subsystems, commands, and button mappings) should be declared here.
 */
public class RobotContainer {
  // The robot's subsystems and commands are defined here...
  private final Drivetrain drivetrain = new Drivetrain();
  private final XRPOnBoardIO onboardIO = new XRPOnBoardIO();
  private final Arm arm = new Arm();

  // Assumes a gamepad plugged into channel 0
  private final Joystick controller = new Joystick(0);

  // Create SmartDashboard chooser for autonomous routines
  private final SendableChooser<Command> chooser = new SendableChooser<>();

  /** The container for the robot. Contains subsystems, OI devices, and commands. */
  public RobotContainer() {
    // Configure the button bindings
    configureButtonBindings();
  }

  /**
   * Use this method to define your button->command mappings. Buttons can be created by
   * instantiating a {@link GenericHID} or one of its subclasses ({@link
   * org.wpilib.driverstation.Joystick} or {@link Gamepad}), and then passing it to a {@link
   * org.wpilib.command2.button.JoystickButton}.
   */
  private void configureButtonBindings() {
    // Default command is arcade drive. This will run unless another command
    // is scheduled over it.
    drivetrain.setDefaultCommand(getArcadeDriveCommand());

    // Example of how to use the onboard IO
    Trigger userButton = new Trigger(onboardIO::getUserButtonPressed);
    userButton
        .onTrue(new PrintCommand("USER Button Pressed"))
        .onFalse(new PrintCommand("USER Button Released"));

    JoystickButton joystickAButton = new JoystickButton(controller, 1);
    joystickAButton
        .onTrue(new InstantCommand(() -> arm.setAngle(45.0), arm))
        .onFalse(new InstantCommand(() -> arm.setAngle(0.0), arm));

    JoystickButton joystickBButton = new JoystickButton(controller, 2);
    joystickBButton
        .onTrue(new InstantCommand(() -> arm.setAngle(90.0), arm))
        .onFalse(new InstantCommand(() -> arm.setAngle(0.0), arm));

    // Setup SmartDashboard options
    chooser.setDefaultOption("Auto Routine Distance", new AutonomousDistance(drivetrain));
    chooser.addOption("Auto Routine Time", new AutonomousTime(drivetrain));
    SmartDashboard.putData(chooser);
  }

  /**
   * Use this to pass the autonomous command to the main {@link Robot} class.
   *
   * @return the command to run in autonomous
   */
  public Command getAutonomousCommand() {
    return chooser.getSelected();
  }

  /**
   * Use this to pass the teleop command to the main {@link Robot} class.
   *
   * @return the command to run in teleop
   */
  public Command getArcadeDriveCommand() {
    return new ArcadeDrive(
        drivetrain, () -> -controller.getRawAxis(1), () -> -controller.getRawAxis(2));
  }
}
