// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.mecanumdrive;

import org.wpilib.drive.MecanumDrive;
import org.wpilib.drivers.motor.PWMSparkMax;
import org.wpilib.driverstation.Joystick;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.imu.OnboardIMU;

/**
 * This is a sample program that uses mecanum drive with a gyro sensor to maintain rotation vectors
 * in relation to the starting orientation of the robot (field-oriented controls).
 */
public class Robot extends TimedRobot {
  private static final int FRONT_LEFT_CHANNEL = 0;
  private static final int REAR_LEFT_CHANNEL = 1;
  private static final int FRONT_RIGHT_CHANNEL = 2;
  private static final int REAR_RIGHT_CHANNEL = 3;
  private static final OnboardIMU.MountOrientation IMU_MOUNT_ORIENTATION =
      OnboardIMU.MountOrientation.FLAT;
  private static final int JOYSTICK_PORT = 0;

  private final MecanumDrive robotDrive;
  private final OnboardIMU imu = new OnboardIMU(IMU_MOUNT_ORIENTATION);
  private final Joystick joystick = new Joystick(JOYSTICK_PORT);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    PWMSparkMax frontLeft = new PWMSparkMax(FRONT_LEFT_CHANNEL);
    PWMSparkMax rearLeft = new PWMSparkMax(REAR_LEFT_CHANNEL);
    PWMSparkMax frontRight = new PWMSparkMax(FRONT_RIGHT_CHANNEL);
    PWMSparkMax rearRight = new PWMSparkMax(REAR_RIGHT_CHANNEL);

    // Invert the right side motors.
    // You may need to change or remove this to match your robot.
    frontRight.setInverted(true);
    rearRight.setInverted(true);

    robotDrive =
        new MecanumDrive(
            frontLeft::setThrottle,
            rearLeft::setThrottle,
            frontRight::setThrottle,
            rearRight::setThrottle);
  }

  /** Mecanum drive is used with the gyro angle as an input. */
  @Override
  public void teleopPeriodic() {
    // Use the joystick Y axis for forward movement, X axis for lateral
    // movement, and Z axis for rotation.
    robotDrive.driveCartesian(
        -joystick.getY(), -joystick.getX(), -joystick.getZ(), imu.getRotation2d());
  }
}
