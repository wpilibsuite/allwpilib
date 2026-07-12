// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.mecanumdrive;

import org.wpilib.drive.MecanumDrive;
import org.wpilib.driverstation.Joystick;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.imu.OnboardIMU;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.util.sendable.SendableRegistry;

/**
 * This is a sample program that uses mecanum drive with a gyro sensor to maintain rotation vectors
 * in relation to the starting orientation of the robot (field-oriented controls).
 */
public class Robot extends TimedRobot {
  private static final int kFrontLeftChannel = 0;
  private static final int kRearLeftChannel = 1;
  private static final int kFrontRightChannel = 2;
  private static final int kRearRightChannel = 3;
  private static final OnboardIMU.MountOrientation kIMUMountOrientation =
      OnboardIMU.MountOrientation.FLAT;
  private static final int kJoystickPort = 0;

  private final MecanumDrive robotDrive;
  private final OnboardIMU imu = new OnboardIMU(kIMUMountOrientation);
  private final Joystick joystick = new Joystick(kJoystickPort);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    PWMSparkMax frontLeft = new PWMSparkMax(kFrontLeftChannel);
    PWMSparkMax rearLeft = new PWMSparkMax(kRearLeftChannel);
    PWMSparkMax frontRight = new PWMSparkMax(kFrontRightChannel);
    PWMSparkMax rearRight = new PWMSparkMax(kRearRightChannel);

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

    SendableRegistry.addChild(robotDrive, frontLeft);
    SendableRegistry.addChild(robotDrive, rearLeft);
    SendableRegistry.addChild(robotDrive, frontRight);
    SendableRegistry.addChild(robotDrive, rearRight);
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
