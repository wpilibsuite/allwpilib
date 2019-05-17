// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.drive;

import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.SpeedControllerGroup;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;
import edu.wpi.first.wpiutil.math.MathUtil;
import java.util.StringJoiner;

/**
 * A class for driving differential drive/skid-steer drive platforms such as the Kit of Parts drive
 * base, "tank drive", or West Coast Drive.
 *
 * <p>These drive bases typically have drop-center / skid-steer with two or more wheels per side
 * (e.g., 6WD or 8WD). This class takes a SpeedController per side. For four and six motor
 * drivetrains, construct and pass in {@link edu.wpi.first.wpilibj.SpeedControllerGroup} instances
 * as follows.
 *
 * <p>Four motor drivetrain:
 *
 * <pre><code>
 * public class Robot {
 *   Spark m_frontLeft = new Spark(1);
 *   Spark m_rearLeft = new Spark(2);
 *   SpeedControllerGroup m_left = new SpeedControllerGroup(m_frontLeft, m_rearLeft);
 *
 *   Spark m_frontRight = new Spark(3);
 *   Spark m_rearRight = new Spark(4);
 *   SpeedControllerGroup m_right = new SpeedControllerGroup(m_frontRight, m_rearRight);
 *
 *   DifferentialDrive m_drive = new DifferentialDrive(m_left, m_right);
 * }
 * </code></pre>
 *
 * <p>Six motor drivetrain:
 *
 * <pre><code>
 * public class Robot {
 *   Spark m_frontLeft = new Spark(1);
 *   Spark m_midLeft = new Spark(2);
 *   Spark m_rearLeft = new Spark(3);
 *   SpeedControllerGroup m_left = new SpeedControllerGroup(m_frontLeft, m_midLeft, m_rearLeft);
 *
 *   Spark m_frontRight = new Spark(4);
 *   Spark m_midRight = new Spark(5);
 *   Spark m_rearRight = new Spark(6);
 *   SpeedControllerGroup m_right = new SpeedControllerGroup(m_frontRight, m_midRight, m_rearRight);
 *
 *   DifferentialDrive m_drive = new DifferentialDrive(m_left, m_right);
 * }
 * </code></pre>
 *
 * <p>A differential drive robot has left and right wheels separated by an arbitrary width.
 *
 * <p>Drive base diagram:
 *
 * <pre>
 * |_______|
 * | |   | |
 *   |   |
 * |_|___|_|
 * |       |
 * </pre>
 *
 * <p>Each drive() function provides different inverse kinematic relations for a differential drive
 * robot. Note that motor direction inversion by the user is usually unnecessary.
 *
 * <p>This library uses the NED axes convention (North-East-Down as external reference in the world
 * frame): http://www.nuclearprojects.com/ins/images/axis_big.png.
 *
 * <p>The positive X axis points ahead, the positive Y axis points right, and the positive Z axis
 * points down. Rotations follow the right-hand rule, so clockwise rotation around the Z axis is
 * positive.
 */
public class DifferentialDrive extends RobotDriveBase implements Sendable {
  private static int instances;

  private final SpeedController m_leftMotor;
  private final SpeedController m_rightMotor;

  private boolean m_reported;

  /**
   * Construct a DifferentialDrive.
   *
   * <p>To pass multiple motors per side, use a {@link SpeedControllerGroup}. If a motor needs to be
   * inverted, do so before passing it in.
   *
   * @param leftMotor Left motor.
   * @param rightMotor Right motor.
   */
  public DifferentialDrive(SpeedController leftMotor, SpeedController rightMotor) {
    verify(leftMotor, rightMotor);
    m_leftMotor = leftMotor;
    m_rightMotor = rightMotor;
    SendableRegistry.addChild(this, m_leftMotor);
    SendableRegistry.addChild(this, m_rightMotor);
    instances++;
    SendableRegistry.addLW(this, "DifferentialDrive", instances);
  }

  /**
   * Verifies that all motors are nonnull, throwing a NullPointerException if any of them are. The
   * exception's error message will specify all null motors, e.g. {@code
   * NullPointerException("leftMotor, rightMotor")}, to give as much information as possible to the
   * programmer.
   *
   * @throws NullPointerException if any of the given motors are null
   */
  @SuppressWarnings("PMD.AvoidThrowingNullPointerException")
  private void verify(SpeedController leftMotor, SpeedController rightMotor) {
    if (leftMotor != null && rightMotor != null) {
      return;
    }
    StringJoiner joiner = new StringJoiner(", ");
    if (leftMotor == null) {
      joiner.add("leftMotor");
    }
    if (rightMotor == null) {
      joiner.add("rightMotor");
    }
    throw new NullPointerException(joiner.toString());
  }

  /**
   * Arcade drive method for differential drive platform. The calculated values will be squared to
   * decrease sensitivity at low speeds.
   *
   * @param xSpeed The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Clockwise is
   *     positive.
   */
  @SuppressWarnings("ParameterName")
  public void arcadeDrive(double xSpeed, double zRotation) {
    arcadeDrive(xSpeed, zRotation, true);
  }

  /**
   * Arcade drive method for differential drive platform.
   *
   * @param xSpeed The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Clockwise is
   *     positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   */
  @SuppressWarnings("ParameterName")
  public void arcadeDrive(double xSpeed, double zRotation, boolean squareInputs) {
    if (!m_reported) {
      HAL.report(
          tResourceType.kResourceType_RobotDrive, 2, tInstances.kRobotDrive2_DifferentialArcade);
      m_reported = true;
    }

    xSpeed = MathUtil.clamp(xSpeed, -1.0, 1.0);
    zRotation = MathUtil.clamp(zRotation, -1.0, 1.0);

    // Square the inputs (while preserving the sign) to increase fine control
    // while permitting full power.
    if (squareInputs) {
      xSpeed = Math.copySign(xSpeed * xSpeed, xSpeed);
      zRotation = Math.copySign(zRotation * zRotation, zRotation);
    }

    double leftSpeed = xSpeed + zRotation;
    double rightSpeed = xSpeed - zRotation;

    // Normalize wheel speeds
    double maxMagnitude = Math.max(Math.abs(leftSpeed), Math.abs(rightSpeed));
    if (maxMagnitude > 1.0) {
      leftSpeed /= maxMagnitude;
      rightSpeed /= maxMagnitude;
    }

    m_leftMotor.set(leftSpeed * m_maxOutput);
    m_rightMotor.set(rightSpeed * m_maxOutput);
  }

  /**
   * Curvature drive method for differential drive platform.
   *
   * <p>The rotation argument controls the curvature of the robot's path rather than its rate of
   * heading change. This makes the robot more controllable at high speeds. Constant-curvature
   * turning can be overridden for turn-in-place maneuvers.
   *
   * @param xSpeed The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Clockwise is
   *     positive.
   * @param allowTurnInPlace If set, overrides constant-curvature turning for turn-in-place
   *     maneuvers.
   */
  @SuppressWarnings({"ParameterName", "PMD.CyclomaticComplexity"})
  public void curvatureDrive(double xSpeed, double zRotation, boolean allowTurnInPlace) {
    if (!m_reported) {
      HAL.report(
          tResourceType.kResourceType_RobotDrive, 2, tInstances.kRobotDrive2_DifferentialCurvature);
      m_reported = true;
    }

    xSpeed = MathUtil.clamp(xSpeed, -1.0, 1.0);
    zRotation = MathUtil.clamp(zRotation, -1.0, 1.0);

    double leftSpeed = 0.0;
    double rightSpeed = 0.0;

    if (allowTurnInPlace) {
      leftSpeed = xSpeed + zRotation;
      rightSpeed = xSpeed - zRotation;
    } else {
      leftSpeed = xSpeed + Math.abs(xSpeed) * zRotation;
      rightSpeed = xSpeed - Math.abs(xSpeed) * zRotation;
    }

    // Normalize wheel speeds
    double maxMagnitude = Math.max(Math.abs(leftSpeed), Math.abs(rightSpeed));
    if (maxMagnitude > 1.0) {
      leftSpeed /= maxMagnitude;
      rightSpeed /= maxMagnitude;
    }

    m_leftMotor.set(leftSpeed * m_maxOutput);
    m_rightMotor.set(rightSpeed * m_maxOutput);
  }

  /**
   * Tank drive method for differential drive platform. The calculated values will be squared to
   * decrease sensitivity at low speeds.
   *
   * @param leftSpeed The robot's left side speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param rightSpeed The robot's right side speed along the X axis [-1.0..1.0]. Forward is
   *     positive.
   */
  public void tankDrive(double leftSpeed, double rightSpeed) {
    tankDrive(leftSpeed, rightSpeed, true);
  }

  /**
   * Tank drive method for differential drive platform.
   *
   * @param leftSpeed The robot left side's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param rightSpeed The robot right side's speed along the X axis [-1.0..1.0]. Forward is
   *     positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   */
  public void tankDrive(double leftSpeed, double rightSpeed, boolean squareInputs) {
    if (!m_reported) {
      HAL.report(
          tResourceType.kResourceType_RobotDrive, 2, tInstances.kRobotDrive2_DifferentialTank);
      m_reported = true;
    }

    leftSpeed = MathUtil.clamp(leftSpeed, -1.0, 1.0);
    rightSpeed = MathUtil.clamp(rightSpeed, -1.0, 1.0);

    // Square the inputs (while preserving the sign) to increase fine control
    // while permitting full power.
    if (squareInputs) {
      leftSpeed = Math.copySign(leftSpeed * leftSpeed, leftSpeed);
      rightSpeed = Math.copySign(rightSpeed * rightSpeed, rightSpeed);
    }

    m_leftMotor.set(leftSpeed * m_maxOutput);
    m_rightMotor.set(rightSpeed * m_maxOutput);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("DifferentialDrive");
    builder.setActuator(true);
    builder.addDoubleProperty("Left Motor Speed", m_leftMotor::get, m_leftMotor::set);
    builder.addDoubleProperty("Right Motor Speed", m_rightMotor::get, m_rightMotor::set);
  }
}
