// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.mecanumdrive;

import org.wpilib.drive.MecanumDrive;
import org.wpilib.driverstation.Joystick;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.motor.PWMSparkMax;

/** This is a demo program showing how to use Mecanum control with the MecanumDrive class. */
public class Robot extends TimedRobot {
  private static final int kFrontLeftChannel = 2;
  private static final int kRearLeftChannel = 3;
  private static final int kFrontRightChannel = 1;
  private static final int kRearRightChannel = 0;

  private static final int kJoystickChannel = 0;

  private final MecanumDrive m_robotDrive;
  private final Joystick m_stick;

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

    m_robotDrive = new MecanumDrive(frontLeft::set, rearLeft::set, frontRight::set, rearRight::set);

    m_stick = new Joystick(kJoystickChannel);
  }

  @Override
  public void teleopPeriodic() {
    // Use the joystick Y axis for forward movement, X axis for lateral
    // movement, and Z axis for rotation.
    m_robotDrive.driveCartesian(-m_stick.getY(), -m_stick.getX(), -m_stick.getZ());
  }
}
