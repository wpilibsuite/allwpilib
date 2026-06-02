// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.xrptimed;

import org.wpilib.drive.DifferentialDrive;
import org.wpilib.driverstation.Joystick;
import org.wpilib.framework.TimedRobot;
import org.wpilib.system.Timer;
import org.wpilib.util.sendable.SendableRegistry;
import org.wpilib.xrp.XRPMotor;

/**
 * The methods in this class are called automatically corresponding to each mode, as described in
 * the TimedRobot documentation. If you change the name of this class or the package after creating
 * this project, you must also update the manifest file in the resource directory.
 */
public class Robot extends TimedRobot {
  private final XRPMotor leftDrive = new XRPMotor(0);
  private final XRPMotor rightDrive = new XRPMotor(1);
  private final DifferentialDrive robotDrive =
      new DifferentialDrive(leftDrive::setThrottle, rightDrive::setThrottle);
  // Assumes a gamepad plugged into channel 0
  private final Joystick controller = new Joystick(0);
  private final Timer timer = new Timer();

  /** Called once at the beginning of the robot program. */
  public Robot() {
    SendableRegistry.addChild(robotDrive, leftDrive);
    SendableRegistry.addChild(robotDrive, rightDrive);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    rightDrive.setInverted(true);
  }

  /** This function is run once each time the robot enters autonomous mode. */
  @Override
  public void autonomousInit() {
    timer.restart();
  }

  /** This function is called periodically during autonomous. */
  @Override
  public void autonomousPeriodic() {
    // Drive for 2 seconds
    if (timer.get() < 2.0) {
      // Drive forwards half speed, make sure to turn input squaring off
      robotDrive.arcadeDrive(0.5, 0.0, false);
    } else {
      robotDrive.stopMotor(); // stop robot
    }
  }

  /** This function is called once each time the robot enters teleoperated mode. */
  @Override
  public void teleopInit() {}

  /** This function is called periodically during teleoperated mode. */
  @Override
  public void teleopPeriodic() {
    robotDrive.arcadeDrive(-controller.getRawAxis(2), -controller.getRawAxis(1));
  }

  /** This function is called once each time the robot enters utility mode. */
  @Override
  public void utilityInit() {}

  /** This function is called periodically during utility mode. */
  @Override
  public void utilityPeriodic() {}
}
