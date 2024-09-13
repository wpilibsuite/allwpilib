// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.drive;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.motorcontrol.MotorController;
import java.util.function.DoubleConsumer;

/**
 * A class for driving differential drive/skid-steer drive platforms such as the Kit of Parts drive
 * base, "tank drive", or West Coast Drive.
 *
 * <p>These drive bases typically have drop-center / skid-steer with two or more wheels per side
 * (e.g., 6WD or 8WD). This class takes a setter per side. For four and six motor drivetrains, use
 * CAN motor controller followers or {@link
 * edu.wpi.first.wpilibj.motorcontrol.PWMMotorController#addFollower(PWMMotorController)}.
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
 * <p>Each drive function provides different inverse kinematic relations for a differential drive
 * robot.
 *
 * <p>This library uses the NWU axes convention (North-West-Up as external reference in the world
 * frame). The positive X axis points ahead, the positive Y axis points to the left, and the
 * positive Z axis points up. Rotations follow the right-hand rule, so counterclockwise rotation
 * around the Z axis is positive.
 *
 * <p>Inputs smaller then {@value edu.wpi.first.wpilibj.drive.RobotDriveBase#kDefaultDeadband} will
 * be set to 0, and larger values will be scaled so that the full range is still used. This deadband
 * value can be changed with {@link #setDeadband}.
 *
 * <p>{@link edu.wpi.first.wpilibj.MotorSafety} is enabled by default. The tankDrive, arcadeDrive,
 * or curvatureDrive methods should be called periodically to avoid Motor Safety timeouts.
 */
public class DifferentialDrive extends RobotDriveBase implements Sendable, AutoCloseable {
  private static int instances;

  private final DoubleConsumer m_leftMotor;
  private final DoubleConsumer m_rightMotor;

  // Used for Sendable property getters
  private double m_leftOutput;
  private double m_rightOutput;

  private boolean m_reported;

  /**
   * Wheel speeds for a differential drive.
   *
   * <p>Uses normalized voltage [-1.0..1.0].
   */
  @SuppressWarnings("MemberName")
  public static class WheelSpeeds {
    /** Left wheel speed. */
    public double left;

    /** Right wheel speed. */
    public double right;

    /** Constructs a WheelSpeeds with zeroes for left and right speeds. */
    public WheelSpeeds() {}

    /**
     * Constructs a WheelSpeeds.
     *
     * @param left The left speed [-1.0..1.0].
     * @param right The right speed [-1.0..1.0].
     */
    public WheelSpeeds(double left, double right) {
      this.left = left;
      this.right = right;
    }
  }

  /**
   * Construct a DifferentialDrive.
   *
   * <p>To pass multiple motors per side, use CAN motor controller followers or {@link
   * edu.wpi.first.wpilibj.motorcontrol.PWMMotorController#addFollower(PWMMotorController)}. If a
   * motor needs to be inverted, do so before passing it in.
   *
   * @param leftMotor Left motor.
   * @param rightMotor Right motor.
   */
  @SuppressWarnings({"removal", "this-escape"})
  public DifferentialDrive(MotorController leftMotor, MotorController rightMotor) {
    this((double output) -> leftMotor.set(output), (double output) -> rightMotor.set(output));
    SendableRegistry.addChild(this, leftMotor);
    SendableRegistry.addChild(this, rightMotor);
  }

  /**
   * Construct a DifferentialDrive.
   *
   * <p>To pass multiple motors per side, use CAN motor controller followers or {@link
   * edu.wpi.first.wpilibj.motorcontrol.PWMMotorController#addFollower(PWMMotorController)}. If a
   * motor needs to be inverted, do so before passing it in.
   *
   * @param leftMotor Left motor setter.
   * @param rightMotor Right motor setter.
   */
  @SuppressWarnings("this-escape")
  public DifferentialDrive(DoubleConsumer leftMotor, DoubleConsumer rightMotor) {
    requireNonNullParam(leftMotor, "leftMotor", "DifferentialDrive");
    requireNonNullParam(rightMotor, "rightMotor", "DifferentialDrive");

    m_leftMotor = leftMotor;
    m_rightMotor = rightMotor;
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
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   */
  public void arcadeDrive(double xSpeed, double zRotation) {
    arcadeDrive(xSpeed, zRotation, true);
  }

  /**
   * Arcade drive method for differential drive platform.
   *
   * @param xSpeed The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   */
  public void arcadeDrive(double xSpeed, double zRotation, boolean squareInputs) {
    if (!m_reported) {
      HAL.report(
          tResourceType.kResourceType_RobotDrive, tInstances.kRobotDrive2_DifferentialArcade, 2);
      m_reported = true;
    }

    xSpeed = MathUtil.applyDeadband(xSpeed, m_deadband);
    zRotation = MathUtil.applyDeadband(zRotation, m_deadband);

    var speeds = arcadeDriveIK(xSpeed, zRotation, squareInputs);

    m_leftOutput = speeds.left * m_maxOutput;
    m_rightOutput = speeds.right * m_maxOutput;

    m_leftMotor.accept(m_leftOutput);
    m_rightMotor.accept(m_rightOutput);

    feed();
  }

  /**
   * Curvature drive method for differential drive platform.
   *
   * <p>The rotation argument controls the curvature of the robot's path rather than its rate of
   * heading change. This makes the robot more controllable at high speeds.
   *
   * @param xSpeed The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The normalized curvature [-1.0..1.0]. Counterclockwise is positive.
   * @param allowTurnInPlace If set, overrides constant-curvature turning for turn-in-place
   *     maneuvers. zRotation will control turning rate instead of curvature.
   */
  public void curvatureDrive(double xSpeed, double zRotation, boolean allowTurnInPlace) {
    if (!m_reported) {
      HAL.report(
          tResourceType.kResourceType_RobotDrive, tInstances.kRobotDrive2_DifferentialCurvature, 2);
      m_reported = true;
    }

    xSpeed = MathUtil.applyDeadband(xSpeed, m_deadband);
    zRotation = MathUtil.applyDeadband(zRotation, m_deadband);

    var speeds = curvatureDriveIK(xSpeed, zRotation, allowTurnInPlace);

    m_leftOutput = speeds.left * m_maxOutput;
    m_rightOutput = speeds.right * m_maxOutput;

    m_leftMotor.accept(m_leftOutput);
    m_rightMotor.accept(m_rightOutput);

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

    m_leftOutput = speeds.left * m_maxOutput;
    m_rightOutput = speeds.right * m_maxOutput;

    m_leftMotor.accept(m_leftOutput);
    m_rightMotor.accept(m_rightOutput);

    feed();
  }

  /**
   * Arcade drive inverse kinematics for differential drive platform.
   *
   * @param xSpeed The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   * @return Wheel speeds [-1.0..1.0].
   */
  public static WheelSpeeds arcadeDriveIK(double xSpeed, double zRotation, boolean squareInputs) {
    xSpeed = MathUtil.clamp(xSpeed, -1.0, 1.0);
    zRotation = MathUtil.clamp(zRotation, -1.0, 1.0);

    // Square the inputs (while preserving the sign) to increase fine control
    // while permitting full power.
    if (squareInputs) {
      xSpeed = Math.copySign(xSpeed * xSpeed, xSpeed);
      zRotation = Math.copySign(zRotation * zRotation, zRotation);
    }

    double leftSpeed = xSpeed - zRotation;
    double rightSpeed = xSpeed + zRotation;

    // Find the maximum possible value of (throttle + turn) along the vector
    // that the joystick is pointing, then desaturate the wheel speeds
    double greaterInput = Math.max(Math.abs(xSpeed), Math.abs(zRotation));
    double lesserInput = Math.min(Math.abs(xSpeed), Math.abs(zRotation));
    if (greaterInput == 0.0) {
      return new WheelSpeeds(0.0, 0.0);
    }
    double saturatedInput = (greaterInput + lesserInput) / greaterInput;
    leftSpeed /= saturatedInput;
    rightSpeed /= saturatedInput;

    return new WheelSpeeds(leftSpeed, rightSpeed);
  }

  /**
   * Curvature drive inverse kinematics for differential drive platform.
   *
   * <p>The rotation argument controls the curvature of the robot's path rather than its rate of
   * heading change. This makes the robot more controllable at high speeds.
   *
   * @param xSpeed The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The normalized curvature [-1.0..1.0]. Counterclockwise is positive.
   * @param allowTurnInPlace If set, overrides constant-curvature turning for turn-in-place
   *     maneuvers. zRotation will control rotation rate around the Z axis instead of curvature.
   * @return Wheel speeds [-1.0..1.0].
   */
  public static WheelSpeeds curvatureDriveIK(
      double xSpeed, double zRotation, boolean allowTurnInPlace) {
    xSpeed = MathUtil.clamp(xSpeed, -1.0, 1.0);
    zRotation = MathUtil.clamp(zRotation, -1.0, 1.0);

    double leftSpeed;
    double rightSpeed;

    if (allowTurnInPlace) {
      leftSpeed = xSpeed - zRotation;
      rightSpeed = xSpeed + zRotation;
    } else {
      leftSpeed = xSpeed - Math.abs(xSpeed) * zRotation;
      rightSpeed = xSpeed + Math.abs(xSpeed) * zRotation;
    }

    // Desaturate wheel speeds
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
   * @return Wheel speeds [-1.0..1.0].
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
    m_leftOutput = 0.0;
    m_rightOutput = 0.0;

    m_leftMotor.accept(0.0);
    m_rightMotor.accept(0.0);

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
    builder.addDoubleProperty("Left Motor Speed", () -> m_leftOutput, m_leftMotor);
    builder.addDoubleProperty("Right Motor Speed", () -> m_rightOutput, m_rightMotor);
  }
}
