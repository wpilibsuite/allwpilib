// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.tankdrivegamepad;

import org.wpilib.drive.DifferentialDrive;
import org.wpilib.driverstation.Gamepad;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.util.sendable.SendableRegistry;

/**
 * This is a demo program showing the use of the DifferentialDrive class. Runs the motors with tank
 * steering and a gamepad.
 */
public class Robot extends TimedRobot {
  private final PWMSparkMax leftMotor = new PWMSparkMax(0);
  private final PWMSparkMax rightMotor = new PWMSparkMax(1);
  private final DifferentialDrive robotDrive =
      new DifferentialDrive(leftMotor::setThrottle, rightMotor::setThrottle);
  private final Gamepad driverController = new Gamepad(0);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    SendableRegistry.addChild(robotDrive, leftMotor);
    SendableRegistry.addChild(robotDrive, rightMotor);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    rightMotor.setInverted(true);
  }

  @Override
  public void teleopPeriodic() {
    // Drive with tank drive.
    // That means that the Y axis of the left stick moves the left side
    // of the robot forward and backward, and the Y axis of the right stick
    // moves the right side of the robot forward and backward.
    robotDrive.tankDrive(-driverController.getLeftY(), -driverController.getRightY());
  }
}
