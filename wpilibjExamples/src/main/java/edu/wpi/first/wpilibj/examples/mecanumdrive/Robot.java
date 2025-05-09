// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.mecanumdrive;

import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.drive.MecanumDrive;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

/** This is a demo program showing how to use Mecanum control with the MecanumDrive class. */
public class Robot extends TimedRobot {
  private static final int FRONT_LEFT_CHANNEL = 2;
  private static final int REAR_LEFT_CHANNEL = 3;
  private static final int FRONT_RIGHT_CHANNEL = 1;
  private static final int REAR_RIGHT_CHANNEL = 0;

  private static final int JOYSTICK_CHANNEL = 0;

  private final MecanumDrive m_robotDrive;
  private final Joystick m_stick;

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

    m_robotDrive = new MecanumDrive(frontLeft::set, rearLeft::set, frontRight::set, rearRight::set);

    m_stick = new Joystick(JOYSTICK_CHANNEL);

    SendableRegistry.addChild(m_robotDrive, frontLeft);
    SendableRegistry.addChild(m_robotDrive, rearLeft);
    SendableRegistry.addChild(m_robotDrive, frontRight);
    SendableRegistry.addChild(m_robotDrive, rearRight);
  }

  @Override
  public void teleopPeriodic() {
    // Use the joystick Y axis for forward movement, X axis for lateral
    // movement, and Z axis for rotation.
    m_robotDrive.driveCartesian(-m_stick.getY(), -m_stick.getX(), -m_stick.getZ());
  }
}
