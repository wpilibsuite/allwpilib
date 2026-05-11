// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.sysidroutine;

import org.wpilib.command2.Command;
import org.wpilib.command2.button.CommandGamepad;
import org.wpilib.command2.button.Trigger;
import org.wpilib.command2.sysid.SysIdRoutine;
import org.wpilib.examples.sysidroutine.Constants.OIConstants;
import org.wpilib.examples.sysidroutine.subsystems.Drive;
import org.wpilib.examples.sysidroutine.subsystems.Shooter;

/**
 * This class is where the bulk of the robot should be declared. Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
 * subsystems, commands, and button mappings) should be declared here.
 */
public class SysIdRoutineBot {
  // The robot's subsystems
  private final Drive drive = new Drive();
  private final Shooter shooter = new Shooter();

  // The driver's controller
  CommandGamepad driverController = new CommandGamepad(OIConstants.kDriverControllerPort);

  /**
   * Use this method to define bindings between conditions and commands. These are useful for
   * automating robot behaviors based on button and sensor input.
   *
   * <p>Should be called in the robot class constructor.
   *
   * <p>Event binding methods are available on the {@link Trigger} class.
   */
  public void configureBindings() {
    // Control the drive with split-stick arcade controls
    drive.setDefaultCommand(
        drive.arcadeDriveCommand(
            () -> -driverController.getLeftY(), () -> -driverController.getRightX()));

    // Bind full set of SysId routine tests to buttons; a complete routine should run each of these
    // once.
    // Using bumpers as a modifier and combining it with the buttons so that we can have both sets
    // of bindings at once
    driverController
        .southFace()
        .and(driverController.rightBumper())
        .whileTrue(drive.sysIdQuasistatic(SysIdRoutine.Direction.kForward));
    driverController
        .eastFace()
        .and(driverController.rightBumper())
        .whileTrue(drive.sysIdQuasistatic(SysIdRoutine.Direction.kReverse));
    driverController
        .westFace()
        .and(driverController.rightBumper())
        .whileTrue(drive.sysIdDynamic(SysIdRoutine.Direction.kForward));
    driverController
        .northFace()
        .and(driverController.rightBumper())
        .whileTrue(drive.sysIdDynamic(SysIdRoutine.Direction.kReverse));

    // Control the shooter wheel with the left trigger
    shooter.setDefaultCommand(shooter.runShooter(driverController::getLeftTriggerAxis));

    driverController
        .southFace()
        .and(driverController.leftBumper())
        .whileTrue(shooter.sysIdQuasistatic(SysIdRoutine.Direction.kForward));
    driverController
        .eastFace()
        .and(driverController.leftBumper())
        .whileTrue(shooter.sysIdQuasistatic(SysIdRoutine.Direction.kReverse));
    driverController
        .westFace()
        .and(driverController.leftBumper())
        .whileTrue(shooter.sysIdDynamic(SysIdRoutine.Direction.kForward));
    driverController
        .northFace()
        .and(driverController.leftBumper())
        .whileTrue(shooter.sysIdDynamic(SysIdRoutine.Direction.kReverse));
  }

  /**
   * Use this to define the command that runs during autonomous.
   *
   * <p>Scheduled during {@link Robot#autonomousInit()}.
   */
  public Command getAutonomousCommand() {
    // Do nothing
    return drive.run(() -> {});
  }
}
