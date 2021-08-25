// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.drive;

import static java.util.Objects.requireNonNull;

import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.SpeedController;

/**
 * A class for driving differential drive/skid-steer drive platforms such as the Kit of Parts drive
 * base, "tank drive", or West Coast Drive.
 *
 * <p>These drive bases typically have drop-center / skid-steer with two or more wheels per side
 * (e.g., 6WD or 8WD). This class takes a MotorController per side. For four and six motor
 * drivetrains, construct and pass in {@link
 * edu.wpi.first.wpilibj.motorcontrol.MotorControllerGroup} instances as follows.
 *
 * <p>Four motor drivetrain:
 *
 * <pre><code>
 * public class Robot {
 *   MotorController m_frontLeft = new PWMVictorSPX(1);
 *   MotorController m_rearLeft = new PWMVictorSPX(2);
 *   MotorControllerGroup m_left = new MotorControllerGroup(m_frontLeft, m_rearLeft);
 *
 *   MotorController m_frontRight = new PWMVictorSPX(3);
 *   MotorController m_rearRight = new PWMVictorSPX(4);
 *   MotorControllerGroup m_right = new MotorControllerGroup(m_frontRight, m_rearRight);
 *
 *   DifferentialDrive m_drive = new DifferentialDrive(m_left, m_right);
 * }
 * </code></pre>
 *
 * <p>Six motor drivetrain:
 *
 * <pre><code>
 * public class Robot {
 *   MotorController m_frontLeft = new PWMVictorSPX(1);
 *   MotorController m_midLeft = new PWMVictorSPX(2);
 *   MotorController m_rearLeft = new PWMVictorSPX(3);
 *   MotorControllerGroup m_left = new MotorControllerGroup(m_frontLeft, m_midLeft, m_rearLeft);
 *
 *   MotorController m_frontRight = new PWMVictorSPX(4);
 *   MotorController m_midRight = new PWMVictorSPX(5);
 *   MotorController m_rearRight = new PWMVictorSPX(6);
 *   MotorControllerGroup m_right = new MotorControllerGroup(m_frontRight, m_midRight, m_rearRight);
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
 * robot. Motor outputs for the right side are negated, so motor direction inversion by the user is
 * usually unnecessary.
 *
 * <p>This library uses the NED axes convention (North-East-Down as external reference in the world
 * frame): http://www.nuclearprojects.com/ins/images/axis_big.png.
 *
 * <p>The positive X axis points ahead, the positive Y axis points right, and the positive Z axis
 * points down. Rotations follow the right-hand rule, so clockwise rotation around the Z axis is
 * positive.
 *
 * <p>Inputs smaller then {@value edu.wpi.first.wpilibj.drive.RobotDriveBase#kDefaultDeadband} will
 * be set to 0, and larger values will be scaled so that the full range is still used. This deadband
 * value can be changed with {@link #setDeadband}.
 */
@SuppressWarnings("removal")
public class DifferentialDrive extends RobotDriveBase implements Sendable, AutoCloseable {
  private static int instances;

  private final SpeedController m_leftMotor;
  private final SpeedController m_rightMotor;

  private boolean m_reported;

  @SuppressWarnings("MemberName")
  public static class WheelSpeeds {
    public double left;
    public double right;

    /** Constructs a WheelSpeeds with zeroes for left and right speeds. */
    public WheelSpeeds() {}

    /**
     * Constructs a WheelSpeeds.
     *
     * @param left The left speed.
     * @param right The right speed.
     */
    public WheelSpeeds(double left, double right) {
      this.left = left;
      this.right = right;
    }
  }

  /**
   * Construct a DifferentialDrive.
   *
   * <p>To pass multiple motors per side, use a {@link
   * edu.wpi.first.wpilibj.motorcontrol.MotorControllerGroup}. If a motor needs to be inverted, do
   * so before passing it in.
   *
   * @param leftMotor Left motor.
   * @param rightMotor Right motor.
   */
  public DifferentialDrive(SpeedController leftMotor, SpeedController rightMotor) {
    requireNonNull(leftMotor, "Left motor cannot be null");
    requireNonNull(rightMotor, "Right motor cannot be null");

    m_leftMotor = leftMotor;
    m_rightMotor = rightMotor;
    SendableRegistry.addChild(this, m_leftMotor);
    SendableRegistry.addChild(this, m_rightMotor);
    instances++;
    SendableRegistry.addLW(this, "DifferentialDrive", instances);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
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
          tResourceType.kResourceType_RobotDrive, tInstances.kRobotDrive2_DifferentialArcade, 2);
      m_reported = true;
    }

    xSpeed = MathUtil.applyDeadband(xSpeed, m_deadband);
    zRotation = MathUtil.applyDeadband(zRotation, m_deadband);

    var speeds = arcadeDriveIK(xSpeed, zRotation, squareInputs);

    m_leftMotor.set(speeds.left * m_maxOutput);
    m_rightMotor.set(speeds.right * m_maxOutput);

    feed();
  }

  /**
   * Curvature drive method for differential drive platform.
   *
   * <p>The rotation argument controls the curvature of the robot's path rather than its rate of
   * heading change. This makes the robot more controllable at high speeds. Also handles the robot's
   * quick turn functionality - "quick turn" overrides constant-curvature turning for turn-in-place
   * maneuvers.
   *
   * @param xSpeed The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Clockwise is
   *     positive.
   * @param allowTurnInPlace If set, overrides constant-curvature turning for turn-in-place
   *     maneuvers.
   */
  @SuppressWarnings("ParameterName")
  public void curvatureDrive(double xSpeed, double zRotation, boolean allowTurnInPlace) {
    if (!m_reported) {
      HAL.report(
          tResourceType.kResourceType_RobotDrive, tInstances.kRobotDrive2_DifferentialCurvature, 2);
      m_reported = true;
    }

    xSpeed = MathUtil.applyDeadband(xSpeed, m_deadband);
    zRotation = MathUtil.applyDeadband(zRotation, m_deadband);

    var speeds = curvatureDriveIK(xSpeed, zRotation, allowTurnInPlace);

    m_leftMotor.set(speeds.left * m_maxOutput);
    m_rightMotor.set(speeds.right * m_maxOutput);

    feed();
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
          tResourceType.kResourceType_RobotDrive, tInstances.kRobotDrive2_DifferentialTank, 2);
      m_reported = true;
    }

    leftSpeed = MathUtil.applyDeadband(leftSpeed, m_deadband);
    rightSpeed = MathUtil.applyDeadband(rightSpeed, m_deadband);

    var speeds = tankDriveIK(leftSpeed, rightSpeed, squareInputs);

    m_leftMotor.set(speeds.left * m_maxOutput);
    m_rightMotor.set(speeds.right * m_maxOutput);

    feed();
  }

  /**
   * Arcade drive inverse kinematics for differential drive platform.
   *
   * @param xSpeed The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Clockwise is
   *     positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   * @return Wheel speeds.
   */
  @SuppressWarnings("ParameterName")
  public static WheelSpeeds arcadeDriveIK(double xSpeed, double zRotation, boolean squareInputs) {
    xSpeed = MathUtil.clamp(xSpeed, -1.0, 1.0);
    zRotation = MathUtil.clamp(zRotation, -1.0, 1.0);

    // Square the inputs (while preserving the sign) to increase fine control
    // while permitting full power.
    if (squareInputs) {
      xSpeed = Math.copySign(xSpeed * xSpeed, xSpeed);
      zRotation = Math.copySign(zRotation * zRotation, zRotation);
    }

    double leftSpeed;
    double rightSpeed;

    double maxInput = Math.copySign(Math.max(Math.abs(xSpeed), Math.abs(zRotation)), xSpeed);

    if (xSpeed >= 0.0) {
      // First quadrant, else second quadrant
      if (zRotation >= 0.0) {
        leftSpeed = maxInput;
        rightSpeed = xSpeed - zRotation;
      } else {
        leftSpeed = xSpeed + zRotation;
        rightSpeed = maxInput;
      }
    } else {
      // Third quadrant, else fourth quadrant
      if (zRotation >= 0.0) {
        leftSpeed = xSpeed + zRotation;
        rightSpeed = maxInput;
      } else {
        leftSpeed = maxInput;
        rightSpeed = xSpeed - zRotation;
      }
    }

    // Normalize the wheel speeds
    double maxMagnitude = Math.max(Math.abs(leftSpeed), Math.abs(rightSpeed));
    if (maxMagnitude > 1.0) {
      leftSpeed /= maxMagnitude;
      rightSpeed /= maxMagnitude;
    }

    return new WheelSpeeds(leftSpeed, rightSpeed);
  }

  /**
   * Curvature drive inverse kinematics for differential drive platform.
   *
   * <p>The rotation argument controls the curvature of the robot's path rather than its rate of
   * heading change. This makes the robot more controllable at high speeds. Also handles the robot's
   * quick turn functionality - "quick turn" overrides constant-curvature turning for turn-in-place
   * maneuvers.
   *
   * @param xSpeed The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Clockwise is
   *     positive.
   * @param allowTurnInPlace If set, overrides constant-curvature turning for turn-in-place
   *     maneuvers.
   * @return Wheel speeds.
   */
  @SuppressWarnings("ParameterName")
  public static WheelSpeeds curvatureDriveIK(
      double xSpeed, double zRotation, boolean allowTurnInPlace) {
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

    return new WheelSpeeds(leftSpeed, rightSpeed);
  }

  /**
   * Tank drive inverse kinematics for differential drive platform.
   *
   * @param leftSpeed The robot left side's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param rightSpeed The robot right side's speed along the X axis [-1.0..1.0]. Forward is
   *     positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   * @return Wheel speeds.
   */
  public static WheelSpeeds tankDriveIK(double leftSpeed, double rightSpeed, boolean squareInputs) {
    leftSpeed = MathUtil.clamp(leftSpeed, -1.0, 1.0);
    rightSpeed = MathUtil.clamp(rightSpeed, -1.0, 1.0);

    // Square the inputs (while preserving the sign) to increase fine control
    // while permitting full power.
    if (squareInputs) {
      leftSpeed = Math.copySign(leftSpeed * leftSpeed, leftSpeed);
      rightSpeed = Math.copySign(rightSpeed * rightSpeed, rightSpeed);
    }

    return new WheelSpeeds(leftSpeed, rightSpeed);
  }

  @Override
  public void stopMotor() {
    m_leftMotor.stopMotor();
    m_rightMotor.stopMotor();
    feed();
  }

  @Override
  public String getDescription() {
    return "DifferentialDrive";
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("DifferentialDrive");
    builder.setActuator(true);
    builder.setSafeState(this::stopMotor);
    builder.addDoubleProperty("Left Motor Speed", m_leftMotor::get, m_leftMotor::set);
    builder.addDoubleProperty(
        "Right Motor Speed", () -> m_rightMotor.get(), x -> m_rightMotor.set(x));
  }
}
