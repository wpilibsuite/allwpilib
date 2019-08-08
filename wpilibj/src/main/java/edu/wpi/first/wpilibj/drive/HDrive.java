/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.drive;

import java.util.StringJoiner;

import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * A class for driving H-drive platforms.
 *
 * <p>A typical H-drive has left and right sides like a differential drive,
 * along with an additional strafe wheel or wheels in the center, oriented
 * 90 degrees from the other wheels. H-drives typically use omni wheels.</p>
 *
 * <p>Drive base diagram:
 * <pre>
 * |________|
 * | |    | |
 *   | -- |
 * |_|____|_|
 * |        |
 * </pre></p>
 *
 * <p>Each Drive() function provides different inverse kinematic relations for an
 * H-drive robot. Motor outputs for the right side are negated, so motor
 * direction inversion by the user is usually unnecessary.</p>
 *
 * <p>This library uses the NED axes convention (North-East-Down as external
 * reference in the world frame):
 * http://www.nuclearprojects.com/ins/images/axis_big.png.</p>
 *
 * <p>The positive X axis points ahead, the positive Y axis points to the right,
 * and the positive Z axis points down. Rotations follow the right-hand rule, so
 * clockwise rotation around the Z axis is positive.</p>
 *
 * <p>Inputs smaller then 0.02 will be set to 0, and larger values will be scaled
 * so that the full range is still used. This deadband value can be changed
 * with SetDeadband().</p>
 *
 */
public class HDrive extends RobotDriveBase {
  private static int instances;

  private final SpeedController m_leftMotor;
  private final SpeedController m_rightMotor;
  private final SpeedController m_strafeMotor;

  private final double m_strafeRotationFactor;

  private boolean m_reported;

  /**
   * Construct an HDrive.
   *
   * <p>If a motor needs to be inverted, do so before passing it in.</p>
   *
   * @param trackWidth the width between the robot's left and right sides
   * @param strafeWheelDistance the distance between the center of rotation and the strafe wheel
   */
  public HDrive(SpeedController leftMotor, SpeedController rightMotor, SpeedController strafeMotor,
                double trackWidth, double strafeWheelDistance) {
    verify(leftMotor, rightMotor, strafeMotor);
    m_leftMotor = leftMotor;
    m_rightMotor = rightMotor;
    m_strafeMotor = strafeMotor;
    m_strafeRotationFactor = trackWidth == 0.0 ? 0.0 : strafeWheelDistance / (trackWidth / 2.0);
    addChild(m_leftMotor);
    addChild(m_rightMotor);
    addChild(m_strafeMotor);
    instances++;
    setName("HDrive", instances);
  }

  /**
   * Construct an HDrive.
   *
   * <p>If a motor needs to be inverted, do so before passing it in.
   */
  public HDrive(SpeedController leftMotor, SpeedController rightMotor,
                SpeedController strafeMotor) {
    this(leftMotor, rightMotor, strafeMotor, 1.0, 0.0);
  }

  /**
   * Verifies that all motors are nonnull, throwing a NullPointerException if any of them are.
   * The exception's error message will specify all null motors, e.g. {@code
   * NullPointerException("frontLeftMotor, rearRightMotor")}, to give as much information as
   * possible to the programmer.
   *
   * @throws NullPointerException if any of the given motors are null
   */
  @SuppressWarnings({"PMD.AvoidThrowingNullPointerException", "PMD.CyclomaticComplexity"})
  private void verify(SpeedController left, SpeedController right,
                      SpeedController strafe) {
    if (left != null && right != null && strafe != null) {
      return;
    }
    StringJoiner joiner = new StringJoiner(", ");
    if (left == null) {
      joiner.add("leftMotor");
    }
    if (right == null) {
      joiner.add("rightMotor");
    }
    if (strafe == null) {
      joiner.add("strafeMotor");
    }
    throw new NullPointerException(joiner.toString());
  }

  /**
   * Drive method for H-drive platform.
   *
   * <p>Angles are measured clockwise from the positive X axis. The robot's speed is independent
   * from its angle or rotation rate.
   *
   * @param ySpeed    The robot's speed along the Y axis [-1.0..1.0]. Right is positive.
   * @param xSpeed    The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Clockwise is
   *                  positive.
   */
  @SuppressWarnings("ParameterName")
  public void driveCartesian(double ySpeed, double xSpeed, double zRotation) {
    driveCartesian(ySpeed, xSpeed, zRotation, 0.0);
  }

  /**
   * Drive method for H-drive platform.
   *
   * <p>Angles are measured clockwise from the positive X axis. The robot's speed is independent
   * from its angle or rotation rate.
   *
   * @param ySpeed    The robot's speed along the Y axis [-1.0..1.0]. Right is positive.
   * @param xSpeed    The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Clockwise is
   *                  positive.
   * @param gyroAngle The current angle reading from the gyro in degrees around the Z axis. Use
   *                  this to implement field-oriented controls.
   */
  @SuppressWarnings("ParameterName")
  public void driveCartesian(double ySpeed, double xSpeed, double zRotation, double gyroAngle) {
    if (!m_reported) {
      HAL.report(tResourceType.kResourceType_RobotDrive, 4,
                 tInstances.kRobotDrive2_HDriveCartesian);
      m_reported = true;
    }

    ySpeed = limit(ySpeed);
    ySpeed = applyDeadband(ySpeed, m_deadband);

    xSpeed = limit(xSpeed);
    xSpeed = applyDeadband(xSpeed, m_deadband);

    zRotation = limit(zRotation);
    zRotation = applyDeadband(zRotation, m_deadband);

    // Compensate for gyro angle.
    Vector2d input = new Vector2d(ySpeed, xSpeed);
    input.rotate(-gyroAngle);

    double[] wheelSpeeds = new double[3];
    wheelSpeeds[MotorType.kLeft.value] = input.x + zRotation + m_strafeRotationFactor * input.y;
    wheelSpeeds[MotorType.kRight.value] = input.x - zRotation - m_strafeRotationFactor * input.y;
    wheelSpeeds[MotorType.kBack.value] = input.y - m_strafeRotationFactor * zRotation;

    normalize(wheelSpeeds);

    m_leftMotor.set(wheelSpeeds[MotorType.kLeft.value] * m_maxOutput);
    m_rightMotor.set(wheelSpeeds[MotorType.kRight.value] * m_maxOutput);
    m_strafeMotor.set(wheelSpeeds[MotorType.kBack.value] * m_maxOutput);

    feed();
  }

  /**
   * Drive method for H-drive platform.
   *
   * <p>Angles are measured counter-clockwise from straight ahead. The speed at which the robot
   * drives (translation) is independent from its angle or rotation rate.
   *
   * @param magnitude The robot's speed at a given angle [-1.0..1.0]. Forward is positive.
   * @param angle     The angle around the Z axis at which the robot drives in degrees [-180..180].
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Clockwise is
   *                  positive.
   */
  @SuppressWarnings("ParameterName")
  public void drivePolar(double magnitude, double angle, double zRotation) {
    if (!m_reported) {
      HAL.report(tResourceType.kResourceType_RobotDrive, 4, tInstances.kRobotDrive2_HDrivePolar);
      m_reported = true;
    }

    driveCartesian(magnitude * Math.sin(angle * (Math.PI / 180.0)),
                   magnitude * Math.cos(angle * (Math.PI / 180.0)), zRotation, 0.0);
  }

  @Override
  public void stopMotor() {
    m_leftMotor.stopMotor();
    m_rightMotor.stopMotor();
    m_strafeMotor.stopMotor();
    feed();
  }

  @Override
  public String getDescription() {
    return "HDrive";
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("HDrive");
    builder.setActuator(true);
    builder.setSafeState(this::stopMotor);
    builder.addDoubleProperty("Left Motor Speed",
        m_leftMotor::get,
        m_leftMotor::set);
    builder.addDoubleProperty("Right Motor Speed",
        () -> m_rightMotor.get(),
        value -> m_rightMotor.set(value));
    builder.addDoubleProperty("Strafe Motor Speed",
        () -> m_strafeMotor.get(),
        value -> m_strafeMotor.set(value));
  }
}
