// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.sysidroutine;

import org.wpilib.command2.Command;
import org.wpilib.command2.CommandScheduler;
import org.wpilib.command2.button.CommandGamepad;
import org.wpilib.command2.button.Trigger;
import org.wpilib.command2.sysid.SysIdRoutine;
import org.wpilib.examples.sysidroutine.Constants.OIConstants;
import org.wpilib.examples.sysidroutine.subsystems.Drive;
import org.wpilib.examples.sysidroutine.subsystems.Shooter;
import org.wpilib.framework.TimedRobot;

/**
 * The methods in this class are called automatically corresponding to each mode, as described in
 * the TimedRobot documentation. If you change the name of this class or the package after creating
 * this project, you must also update the Main.java file in the project.
 */
public class Robot extends TimedRobot {
  private Command autonomousCommand;

  // The robot's subsystems
  private final Drive drive = new Drive();
  private final Shooter shooter = new Shooter();

  // The driver's controller
  CommandGamepad driverController = new CommandGamepad(OIConstants.DRIVER_CONTROLLER_PORT);

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  public Robot() {
    // Configure default commands and condition bindings on robot startup
    configureBindings();
  }

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
        .faceDown()
        .and(driverController.rightBumper())
        .whileTrue(drive.sysIdQuasistatic(SysIdRoutine.Direction.FORWARD));
    driverController
        .faceRight()
        .and(driverController.rightBumper())
        .whileTrue(drive.sysIdQuasistatic(SysIdRoutine.Direction.REVERSE));
    driverController
        .faceLeft()
        .and(driverController.rightBumper())
        .whileTrue(drive.sysIdDynamic(SysIdRoutine.Direction.FORWARD));
    driverController
        .faceUp()
        .and(driverController.rightBumper())
        .whileTrue(drive.sysIdDynamic(SysIdRoutine.Direction.REVERSE));

    // Control the shooter wheel with the left trigger
    shooter.setDefaultCommand(shooter.runShooter(driverController::getLeftTrigger));

    driverController
        .faceDown()
        .and(driverController.leftBumper())
        .whileTrue(shooter.sysIdQuasistatic(SysIdRoutine.Direction.FORWARD));
    driverController
        .faceRight()
        .and(driverController.leftBumper())
        .whileTrue(shooter.sysIdQuasistatic(SysIdRoutine.Direction.REVERSE));
    driverController
        .faceLeft()
        .and(driverController.leftBumper())
        .whileTrue(shooter.sysIdDynamic(SysIdRoutine.Direction.FORWARD));
    driverController
        .faceUp()
        .and(driverController.leftBumper())
        .whileTrue(shooter.sysIdDynamic(SysIdRoutine.Direction.REVERSE));
  }

  /**
   * This function is called every robot packet, no matter the mode. Use this for items like
   * diagnostics that you want ran during disabled, autonomous, teleoperated and utility.
   *
   * <p>This runs after the mode specific periodic functions, but before LiveWindow and
   * SmartDashboard integrated updating.
   */
  @Override
  public void robotPeriodic() {
    // Runs the Scheduler.  This is responsible for polling buttons, adding newly-scheduled
    // commands, running already-scheduled commands, removing finished or interrupted commands,
    // and running subsystem periodic() methods.  This must be called from the robot's periodic
    // block in order for anything in the Command-based framework to work.
    CommandScheduler.getInstance().run();
  }

  /** This function is called each time Disabled mode starts. */
  @Override
  public void disabledStart() {}

  @Override
  public void disabledPeriodic() {}

  @Override
  public void autonomousStart() {
    // Do nothing
    autonomousCommand = drive.run(() -> {});
    CommandScheduler.getInstance().schedule(autonomousCommand);
  }

  /** This function is called periodically during autonomous. */
  @Override
  public void autonomousPeriodic() {}

  @Override
  public void autonomousEnd() {
    // This makes sure that the autonomous stops running when
    // autonomous mode ends. If you want the autonomous to
    // continue until interrupted by another command, remove
    // this line or comment it out.
    if (autonomousCommand != null) {
      autonomousCommand.cancel();
    }
  }

  /** This function is called each time teleoperated mode starts. */
  @Override
  public void teleopStart() {}

  /** This function is called periodically during teleoperated mode. */
  @Override
  public void teleopPeriodic() {}

  @Override
  public void utilityStart() {
    // Cancels all running commands at the start of utility mode.
    CommandScheduler.getInstance().cancelAll();
  }

  /** This function is called periodically during utility mode. */
  @Override
  public void utilityPeriodic() {}
}
