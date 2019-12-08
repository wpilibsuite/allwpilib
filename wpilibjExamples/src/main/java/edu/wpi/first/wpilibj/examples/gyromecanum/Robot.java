/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gyromecanum;

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.PWMVictorSPX;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.drive.MecanumDrive;

import static edu.wpi.first.wpilibj.examples.gyromecanum.Constants.DriveConstants.kFrontLeftChannel;
import static edu.wpi.first.wpilibj.examples.gyromecanum.Constants.DriveConstants.kFrontRightChannel;
import static edu.wpi.first.wpilibj.examples.gyromecanum.Constants.DriveConstants.kRearLeftChannel;
import static edu.wpi.first.wpilibj.examples.gyromecanum.Constants.DriveConstants.kRearRightChannel;
import static edu.wpi.first.wpilibj.examples.gyromecanum.Constants.DriveConstants.kVoltsPerDegreePerSecond;
import static edu.wpi.first.wpilibj.examples.gyromecanum.Constants.OIConstants.kJoystickPort;

/**
 * This is a sample program that uses mecanum drive with a gyro sensor to
 * maintain rotation vectorsin relation to the starting orientation of the robot
 * (field-oriented controls).
 */
public class Robot extends TimedRobot {
  private MecanumDrive m_robotDrive;
  private final AnalogGyro m_gyro = new AnalogGyro(0);
  private final Joystick m_joystick = new Joystick(kJoystickPort);

  @Override
  public void robotInit() {
    PWMVictorSPX frontLeft = new PWMVictorSPX(kFrontLeftChannel);
    PWMVictorSPX rearLeft = new PWMVictorSPX(kRearLeftChannel);
    PWMVictorSPX frontRight = new PWMVictorSPX(kFrontRightChannel);
    PWMVictorSPX rearRight = new PWMVictorSPX(kRearRightChannel);

    // Invert the left side motors.
    // You may need to change or remove this to match your robot.
    frontLeft.setInverted(true);
    rearLeft.setInverted(true);

    m_robotDrive = new MecanumDrive(frontLeft, rearLeft, frontRight, rearRight);

    m_gyro.setSensitivity(kVoltsPerDegreePerSecond);
  }

  /**
   * Mecanum drive is used with the gyro angle as an input.
   */
  @Override
  public void teleopPeriodic() {
    m_robotDrive.driveCartesian(m_joystick.getX(), m_joystick.getY(),
        m_joystick.getZ(), m_gyro.getAngle());
  }
}
