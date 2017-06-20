/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.drive;

import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tInstances;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;

/**
 * A class for driving Mecanum drive platforms.
 *
 * <p>Mecanum drives are rectangular with one wheel on each corner. Each wheel has rollers toed in
 * 45 degrees toward the front or back. When looking at the wheels from the top, the roller axles
 * should form an X across the robot. Each drive() function provides different inverse kinematic
 * relations for a Mecanum drive robot.
 *
 * <p>Drive base diagram:
 * <pre>
 * \\_______/
 * \\ |   | /
 *   |   |
 * /_|___|_\\
 * /       \\
 * </pre>
 *
 * <p>Each drive() function provides different inverse kinematic relations for a Mecanum drive
 * robot. Motor outputs for the right side are negated, so motor direction inversion by the user is
 * usually unnecessary.
 */
public class MecanumDrive extends RobotDriveBase {
  private SpeedController m_frontLeftMotor;
  private SpeedController m_rearLeftMotor;
  private SpeedController m_frontRightMotor;
  private SpeedController m_rearRightMotor;

  private boolean m_reported = false;

  /**
   * Construct a MecanumDrive.
   *
   * <p>If a motor needs to be inverted, do so before passing it in.
   */
  public MecanumDrive(SpeedController frontLeftMotor, SpeedController rearLeftMotor,
                      SpeedController frontRightMotor, SpeedController rearRightMotor) {
    m_frontLeftMotor = frontLeftMotor;
    m_rearLeftMotor = rearLeftMotor;
    m_frontRightMotor = frontRightMotor;
    m_rearRightMotor = rearRightMotor;
  }

  /**
   * Drive method for Mecanum platform.
   *
   * @param x         The speed that the robot should drive in the X direction. [-1.0..1.0]
   * @param y         The speed that the robot should drive in the Y direction. [-1.0..1.0]
   * @param rotation  The rate of rotation for the robot that is completely independent of the
   *                  translation. [-1.0..1.0]
   */
  @SuppressWarnings("ParameterName")
  public void driveCartesian(double x, double y, double rotation) {
    driveCartesian(x, y, rotation, 0.0);
  }

  /**
   * Drive method for Mecanum platform.
   *
   * @param x         The speed that the robot should drive in the X direction. [-1.0..1.0]
   * @param y         The speed that the robot should drive in the Y direction. [-1.0..1.0]
   * @param rotation  The rate of rotation for the robot that is completely independent of the
   *                  translation. [-1.0..1.0]
   * @param gyroAngle The current angle reading from the gyro. Use this to implement field-oriented
   *                  controls.
   */
  @SuppressWarnings("ParameterName")
  public void driveCartesian(double x, double y, double rotation, double gyroAngle) {
    if (!m_reported) {
      HAL.report(tResourceType.kResourceType_RobotDrive, 4,
                 tInstances.kRobotDrive_MecanumCartesian);
      m_reported = true;
    }

    x = limit(x);
    x = applyDeadband(x, m_deadband);

    y = limit(y);
    y = applyDeadband(y, m_deadband);

    // Compensate for gyro angle.
    Vector2d input = new Vector2d(x, y);
    input.rotate(gyroAngle);

    double[] wheelSpeeds = new double[4];
    wheelSpeeds[MotorType.kFrontLeft.value] = input.x + input.y + rotation;
    wheelSpeeds[MotorType.kFrontRight.value] = input.x - input.y + rotation;
    wheelSpeeds[MotorType.kRearLeft.value] = -input.x + input.y + rotation;
    wheelSpeeds[MotorType.kRearRight.value] = -input.x - input.y + rotation;

    normalize(wheelSpeeds);

    m_frontLeftMotor.set(wheelSpeeds[MotorType.kFrontLeft.value] * m_maxOutput);
    m_frontRightMotor.set(wheelSpeeds[MotorType.kFrontRight.value] * m_maxOutput);
    m_rearLeftMotor.set(wheelSpeeds[MotorType.kRearLeft.value] * m_maxOutput);
    m_rearRightMotor.set(wheelSpeeds[MotorType.kRearRight.value] * m_maxOutput);

    m_safetyHelper.feed();
  }

  /**
   * Drive method for Mecanum platform.
   *
   * @param magnitude The speed that the robot should drive in a given direction. [-1.0..1.0]
   * @param angle     The direction the robot should drive in degrees. 0.0 is straight ahead. The
   *                  direction and maginitude are independent of the rotation rate.
   * @param rotation  The rate of rotation for the robot that is completely independent of the
   *                  magnitude or direction. [-1.0..1.0]
   */
  public void drivePolar(double magnitude, double angle, double rotation) {
    if (!m_reported) {
      HAL.report(tResourceType.kResourceType_RobotDrive, 4, tInstances.kRobotDrive_MecanumPolar);
      m_reported = true;
    }

    // Normalized for full power along the Cartesian axes.
    magnitude = limit(magnitude) * Math.sqrt(2.0);

    driveCartesian(magnitude * Math.cos(angle * (Math.PI / 180.0)),
                    magnitude * Math.sin(angle * (Math.PI / 180.0)), rotation, 0.0);
  }

  @Override
  public void stopMotor() {
    m_frontLeftMotor.stopMotor();
    m_frontRightMotor.stopMotor();
    m_rearLeftMotor.stopMotor();
    m_rearRightMotor.stopMotor();
    m_safetyHelper.feed();
  }

  @Override
  public String getDescription() {
    return "MecanumDrive";
  }
}
