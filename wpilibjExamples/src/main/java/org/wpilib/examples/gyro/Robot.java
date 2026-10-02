// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.gyro;

import org.wpilib.drive.DifferentialDrive;
import org.wpilib.drivers.motor.PWMSparkMax;
import org.wpilib.driverstation.Joystick;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.imu.OnboardIMU;

/**
 * This is a sample program to demonstrate how to use a gyro sensor to make a robot drive straight.
 * This program uses a joystick to drive forwards and backwards while the gyro is used for direction
 * keeping.
 */
public class Robot extends TimedRobot {
  private static final double ANGLE_SETPOINT = 0.0;
  private static final double kP = 0.005; // proportional turning constant

  private static final int LEFT_MOTOR_PORT = 0;
  private static final int RIGHT_MOTOR_PORT = 1;
  private static final OnboardIMU.MountOrientation IMU_MOUNT_ORIENTATION =
      OnboardIMU.MountOrientation.FLAT;
  private static final int JOYSTICK_PORT = 0;

  private final PWMSparkMax leftDrive = new PWMSparkMax(LEFT_MOTOR_PORT);
  private final PWMSparkMax rightDrive = new PWMSparkMax(RIGHT_MOTOR_PORT);
  private final DifferentialDrive robotDrive =
      new DifferentialDrive(leftDrive::setThrottle, rightDrive::setThrottle);
  private final OnboardIMU imu = new OnboardIMU(IMU_MOUNT_ORIENTATION);
  private final Joystick joystick = new Joystick(JOYSTICK_PORT);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    rightDrive.setInverted(true);
  }

  /**
   * The motor velocity is set from the joystick while the DifferentialDrive turning value is
   * assigned from the error between the setpoint and the gyro angle.
   */
  @Override
  public void teleopPeriodic() {
    double turningValue = (ANGLE_SETPOINT - imu.getRotation2d().getDegrees()) * kP;
    robotDrive.arcadeDrive(-joystick.getY(), -turningValue);
  }
}
