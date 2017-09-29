/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.drive;

import edu.wpi.first.wpilibj.SpeedController;
// import edu.wpi.first.wpilibj.hal.FRCNetComm.tInstances;
// import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
// import edu.wpi.first.wpilibj.hal.HAL;

/**
 * A class for driving Killough drive platforms.
 *
 * <p>Killough drives are triangular with one omni wheel on each corner.
 *
 * <p>Drive base diagram:
 * <pre>
 *  /_____\
 * / \   / \
 *    \ /
 *    ---
 * </pre>
 *
 * <p>Each drive() function provides different inverse kinematic relations for a Killough drive. The
 * default wheel vectors are parallel to their respective opposite sides, but can be overridden. See
 * the constructor for more information.
 */
public class KilloughDrive extends RobotDriveBase {
  private SpeedController m_leftMotor;
  private SpeedController m_rightMotor;
  private SpeedController m_backMotor;

  private Vector2d m_leftVec;
  private Vector2d m_rightVec;
  private Vector2d m_backVec;

  private boolean m_reported = false;

  /**
   * Construct a Killough drive with the given motors and default motor angles.
   *
   * <p>The default motor angles are 120, 60, and 270 degrees for the left, right, and back motors
   * respectively, which make the wheels on each corner parallel to their respective opposite sides.
   *
   * <p>If a motor needs to be inverted, do so before passing it in.
   *
   * @param leftMotor  The motor on the left corner.
   * @param rightMotor The motor on the right corner.
   * @param backMotor  The motor on the back corner.
   */
  public KilloughDrive(SpeedController leftMotor, SpeedController rightMotor,
                       SpeedController backMotor) {
    this(leftMotor, rightMotor, backMotor, 120.0, 60.0, 270.0);
  }

  /**
   * Construct a Killough drive with the given motors.
   *
   * <p>Angles are measured in counter-clockwise degrees where zero degrees is straight ahead.
   *
   * @param leftMotor       The motor on the left corner.
   * @param rightMotor      The motor on the right corner.
   * @param backMotor       The motor on the back corner.
   * @param leftMotorAngle  The angle of the left wheel's forward direction of travel.
   * @param rightMotorAngle The angle of the right wheel's forward direction of travel.
   * @param backMotorAngle  The angle of the back wheel's forward direction of travel.
   */
  public KilloughDrive(SpeedController leftMotor, SpeedController rightMotor,
                       SpeedController backMotor, double leftMotorAngle, double rightMotorAngle,
                       double backMotorAngle) {
    m_leftMotor = leftMotor;
    m_rightMotor = rightMotor;
    m_backMotor = backMotor;
    m_leftVec = new Vector2d(Math.cos(leftMotorAngle * (Math.PI / 180.0)),
                             Math.sin(leftMotorAngle * (Math.PI / 180.0)));
    m_rightVec = new Vector2d(Math.cos(rightMotorAngle * (Math.PI / 180.0)),
                              Math.sin(rightMotorAngle * (Math.PI / 180.0)));
    m_backVec = new Vector2d(Math.cos(backMotorAngle * (Math.PI / 180.0)),
                             Math.sin(backMotorAngle * (Math.PI / 180.0)));
  }

  /**
   * Drive method for Killough platform.
   *
   * @param x         The speed that the robot should drive in the X direction.
   *                  [-1.0..1.0]
   * @param y         The speed that the robot should drive in the Y direction.
   *                  [-1.0..1.0]
   * @param rotation  The rate of rotation for the robot that is completely independent of the
   *                  translation. [-1.0..1.0]
   */
  @SuppressWarnings("ParameterName")
  public void driveCartesian(double x, double y, double rotation) {
    driveCartesian(x, y, rotation, 0.0);
  }

  /**
   * Drive method for Killough platform.
   *
   * @param x         The speed that the robot should drive in the X direction.
   *                  [-1.0..1.0]
   * @param y         The speed that the robot should drive in the Y direction.
   *                  [-1.0..1.0]
   * @param rotation  The rate of rotation for the robot that is completely independent of the
   *                  translation. [-1.0..1.0]
   * @param gyroAngle The current angle reading from the gyro.  Use this to implement
   *                  field-oriented controls.
   */
  @SuppressWarnings("ParameterName")
  public void driveCartesian(double x, double y, double rotation,
                       double gyroAngle) {
    if (!m_reported) {
      // HAL.report(tResourceType.kResourceType_RobotDrive, 3,
      //            tInstances.kRobotDrive_KilloughCartesian);
      m_reported = true;
    }

    x = limit(x);
    x = applyDeadband(x, m_deadband);

    y = limit(y);
    y = applyDeadband(y, m_deadband);

    // Compensate for gyro angle.
    Vector2d input = new Vector2d(x, y);
    input.rotate(gyroAngle);

    double[] wheelSpeeds = new double[3];
    wheelSpeeds[MotorType.kLeft.value] = input.scalarProject(m_leftVec) + rotation;
    wheelSpeeds[MotorType.kRight.value] = input.scalarProject(m_rightVec) + rotation;
    wheelSpeeds[MotorType.kBack.value] = input.scalarProject(m_backVec) + rotation;

    normalize(wheelSpeeds);

    m_leftMotor.set(wheelSpeeds[MotorType.kLeft.value] * m_maxOutput);
    m_rightMotor.set(wheelSpeeds[MotorType.kRight.value] * m_maxOutput);
    m_backMotor.set(wheelSpeeds[MotorType.kBack.value] * m_maxOutput);

    m_safetyHelper.feed();
  }

  /**
   * Drive method for Killough platform.
   *
   * @param magnitude The speed that the robot should drive in a given direction. [-1.0..1.0]
   * @param angle     The direction the robot should drive in degrees. 0.0 is straight ahead. The
   *                  direction and maginitude are independent of the rotation rate.
   * @param rotation  The rate of rotation for the robot that is completely independent of the
   *                  magnitude or direction. [-1.0..1.0]
   */
  public void drivePolar(double magnitude, double angle, double rotation) {
    if (!m_reported) {
      // HAL.report(tResourceType.kResourceType_RobotDrive, 3,
      //            tInstances.kRobotDrive_KilloughPolar);
      m_reported = true;
    }

    // Normalized for full power along the Cartesian axes.
    magnitude = limit(magnitude) * Math.sqrt(2.0);

    driveCartesian(magnitude * Math.cos(angle * (Math.PI / 180.0)),
                    magnitude * Math.sin(angle * (Math.PI / 180.0)), rotation, 0.0);
  }

  @Override
  public void stopMotor() {
    m_leftMotor.stopMotor();
    m_rightMotor.stopMotor();
    m_backMotor.stopMotor();
    m_safetyHelper.feed();
  }

  @Override
  public String getDescription() {
    return "KilloughDrive";
  }
}
