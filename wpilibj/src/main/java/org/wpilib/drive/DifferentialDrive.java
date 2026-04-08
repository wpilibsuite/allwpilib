// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.drive;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.function.DoubleConsumer;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.motor.MotorController;
import org.wpilib.math.util.MathUtil;
import org.wpilib.util.sendable.Sendable;
import org.wpilib.util.sendable.SendableBuilder;
import org.wpilib.util.sendable.SendableRegistry;

/**
 * A class for driving differential drive/skid-steer drive platforms such as the Kit of Parts drive
 * base, "tank drive", or West Coast Drive.
 *
 * <p>These drive bases typically have drop-center / skid-steer with two or more wheels per side
 * (e.g., 6WD or 8WD). This class takes a setter per side. For four and six motor drivetrains, use
 * CAN motor controller followers or {@link
 * org.wpilib.hardware.motor.PWMMotorController#addFollower(PWMMotorController)}.
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
 * <p>Inputs smaller then {@value org.wpilib.drive.RobotDriveBase#kDefaultDeadband} will be set to
 * 0, and larger values will be scaled so that the full range is still used. This deadband value can
 * be changed with {@link #setDeadband}.
 *
 * <p>{@link org.wpilib.hardware.motor.MotorSafety} is enabled by default. The tankDrive,
 * arcadeDrive, or curvatureDrive methods should be called periodically to avoid Motor Safety
 * timeouts.
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
   * Wheel velocities for a differential drive.
   *
   * <p>Uses normalized voltage [-1.0..1.0].
   */
  @SuppressWarnings("MemberName")
  public static class WheelVelocities {
    /** Left wheel velocity. */
    public double left;

    /** Right wheel velocity. */
    public double right;

    /** Constructs a WheelVelocities with zeroes for left and right velocities. */
    public WheelVelocities() {}

    /**
     * Constructs a WheelVelocities.
     *
     * @param left The left velocity [-1.0..1.0].
     * @param right The right velocity [-1.0..1.0].
     */
    public WheelVelocities(double left, double right) {
      this.left = left;
      this.right = right;
    }
  }

  /**
   * Construct a DifferentialDrive.
   *
   * <p>To pass multiple motors per side, use CAN motor controller followers or {@link
   * org.wpilib.hardware.motor.PWMMotorController#addFollower(PWMMotorController)}. If a motor needs
   * to be inverted, do so before passing it in.
   *
   * @param leftMotor Left motor.
   * @param rightMotor Right motor.
   */
  @SuppressWarnings({"removal", "this-escape"})
  public DifferentialDrive(MotorController leftMotor, MotorController rightMotor) {
    this(
        (double output) -> leftMotor.setDutyCycle(output),
        (double output) -> rightMotor.setDutyCycle(output));
    SendableRegistry.addChild(this, leftMotor);
    SendableRegistry.addChild(this, rightMotor);
  }

  /**
   * Construct a DifferentialDrive.
   *
   * <p>To pass multiple motors per side, use CAN motor controller followers or {@link
   * org.wpilib.hardware.motor.PWMMotorController#addFollower(PWMMotorController)}. If a motor needs
   * to be inverted, do so before passing it in.
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
    SendableRegistry.add(this, "DifferentialDrive", instances);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
  }

  /**
   * Arcade drive method for differential drive platform. The calculated values will be squared to
   * decrease sensitivity at low velocities.
   *
   * @param xVelocity The robot's velocity along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   */
  public void arcadeDrive(double xVelocity, double zRotation) {
    arcadeDrive(xVelocity, zRotation, true);
  }

  /**
   * Arcade drive method for differential drive platform.
   *
   * @param xVelocity The robot's velocity along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   * @param squareInputs If set, decreases the input sensitivity at low velocities.
   */
  public void arcadeDrive(double xVelocity, double zRotation, boolean squareInputs) {
    if (!m_reported) {
      HAL.reportUsage("RobotDrive", "DifferentialArcade");
      m_reported = true;
    }

    xVelocity = MathUtil.applyDeadband(xVelocity, m_deadband);
    zRotation = MathUtil.applyDeadband(zRotation, m_deadband);

    var velocities = arcadeDriveIK(xVelocity, zRotation, squareInputs);

    m_leftOutput = velocities.left * m_maxOutput;
    m_rightOutput = velocities.right * m_maxOutput;

    m_leftMotor.accept(m_leftOutput);
    m_rightMotor.accept(m_rightOutput);

    feed();
  }

  /**
   * Curvature drive method for differential drive platform.
   *
   * <p>The rotation argument controls the curvature of the robot's path rather than its rate of
   * heading change. This makes the robot more controllable at high velocities.
   *
   * @param xVelocity The robot's velocity along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The normalized curvature [-1.0..1.0]. Counterclockwise is positive.
   * @param allowTurnInPlace If set, overrides constant-curvature turning for turn-in-place
   *     maneuvers. zRotation will control turning rate instead of curvature.
   */
  public void curvatureDrive(double xVelocity, double zRotation, boolean allowTurnInPlace) {
    if (!m_reported) {
      HAL.reportUsage("RobotDrive", "DifferentialCurvature");
      m_reported = true;
    }

    xVelocity = MathUtil.applyDeadband(xVelocity, m_deadband);
    zRotation = MathUtil.applyDeadband(zRotation, m_deadband);

    var velocities = curvatureDriveIK(xVelocity, zRotation, allowTurnInPlace);

    m_leftOutput = velocities.left * m_maxOutput;
    m_rightOutput = velocities.right * m_maxOutput;

    m_leftMotor.accept(m_leftOutput);
    m_rightMotor.accept(m_rightOutput);

    feed();
  }

  /**
   * Tank drive method for differential drive platform. The calculated values will be squared to
   * decrease sensitivity at low velocities.
   *
   * @param leftVelocity The robot's left side velocity along the X axis [-1.0..1.0]. Forward is
   *     positive.
   * @param rightVelocity The robot's right side velocity along the X axis [-1.0..1.0]. Forward is
   *     positive.
   */
  public void tankDrive(double leftVelocity, double rightVelocity) {
    tankDrive(leftVelocity, rightVelocity, true);
  }

  /**
   * Tank drive method for differential drive platform.
   *
   * @param leftVelocity The robot left side's velocity along the X axis [-1.0..1.0]. Forward is
   *     positive.
   * @param rightVelocity The robot right side's velocity along the X axis [-1.0..1.0]. Forward is
   *     positive.
   * @param squareInputs If set, decreases the input sensitivity at low velocities.
   */
  public void tankDrive(double leftVelocity, double rightVelocity, boolean squareInputs) {
    if (!m_reported) {
      HAL.reportUsage("RobotDrive", "DifferentialTank");
      m_reported = true;
    }

    leftVelocity = MathUtil.applyDeadband(leftVelocity, m_deadband);
    rightVelocity = MathUtil.applyDeadband(rightVelocity, m_deadband);

    var velocities = tankDriveIK(leftVelocity, rightVelocity, squareInputs);

    m_leftOutput = velocities.left * m_maxOutput;
    m_rightOutput = velocities.right * m_maxOutput;

    m_leftMotor.accept(m_leftOutput);
    m_rightMotor.accept(m_rightOutput);

    feed();
  }

  /**
   * Arcade drive inverse kinematics for differential drive platform.
   *
   * @param xVelocity The robot's velocity along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   * @param squareInputs If set, decreases the input sensitivity at low velocities.
   * @return Wheel velocities [-1.0..1.0].
   */
  public static WheelVelocities arcadeDriveIK(
      double xVelocity, double zRotation, boolean squareInputs) {
    xVelocity = Math.clamp(xVelocity, -1.0, 1.0);
    zRotation = Math.clamp(zRotation, -1.0, 1.0);

    // Square the inputs (while preserving the sign) to increase fine control
    // while permitting full power.
    if (squareInputs) {
      xVelocity = MathUtil.copyDirectionPow(xVelocity, 2);
      zRotation = MathUtil.copyDirectionPow(zRotation, 2);
    }

    double leftVelocity = xVelocity - zRotation;
    double rightVelocity = xVelocity + zRotation;

    // Find the maximum possible value of (throttle + turn) along the vector
    // that the joystick is pointing, then desaturate the wheel velocities
    double greaterInput = Math.max(Math.abs(xVelocity), Math.abs(zRotation));
    double lesserInput = Math.min(Math.abs(xVelocity), Math.abs(zRotation));
    if (greaterInput == 0.0) {
      return new WheelVelocities(0.0, 0.0);
    }
    double saturatedInput = (greaterInput + lesserInput) / greaterInput;
    leftVelocity /= saturatedInput;
    rightVelocity /= saturatedInput;

    return new WheelVelocities(leftVelocity, rightVelocity);
  }

  /**
   * Curvature drive inverse kinematics for differential drive platform.
   *
   * <p>The rotation argument controls the curvature of the robot's path rather than its rate of
   * heading change. This makes the robot more controllable at high velocities.
   *
   * @param xVelocity The robot's velocity along the X axis [-1.0..1.0]. Forward is positive.
   * @param zRotation The normalized curvature [-1.0..1.0]. Counterclockwise is positive.
   * @param allowTurnInPlace If set, overrides constant-curvature turning for turn-in-place
   *     maneuvers. zRotation will control rotation rate around the Z axis instead of curvature.
   * @return Wheel velocities [-1.0..1.0].
   */
  public static WheelVelocities curvatureDriveIK(
      double xVelocity, double zRotation, boolean allowTurnInPlace) {
    xVelocity = Math.clamp(xVelocity, -1.0, 1.0);
    zRotation = Math.clamp(zRotation, -1.0, 1.0);

    double leftVelocity;
    double rightVelocity;

    if (allowTurnInPlace) {
      leftVelocity = xVelocity - zRotation;
      rightVelocity = xVelocity + zRotation;
    } else {
      leftVelocity = xVelocity - Math.abs(xVelocity) * zRotation;
      rightVelocity = xVelocity + Math.abs(xVelocity) * zRotation;
    }

    // Desaturate wheel velocities
    double maxMagnitude = Math.max(Math.abs(leftVelocity), Math.abs(rightVelocity));
    if (maxMagnitude > 1.0) {
      leftVelocity /= maxMagnitude;
      rightVelocity /= maxMagnitude;
    }

    return new WheelVelocities(leftVelocity, rightVelocity);
  }

  /**
   * Tank drive inverse kinematics for differential drive platform.
   *
   * @param leftVelocity The robot left side's velocity along the X axis [-1.0..1.0]. Forward is
   *     positive.
   * @param rightVelocity The robot right side's velocity along the X axis [-1.0..1.0]. Forward is
   *     positive.
   * @param squareInputs If set, decreases the input sensitivity at low velocities.
   * @return Wheel velocities [-1.0..1.0].
   */
  public static WheelVelocities tankDriveIK(
      double leftVelocity, double rightVelocity, boolean squareInputs) {
    leftVelocity = Math.clamp(leftVelocity, -1.0, 1.0);
    rightVelocity = Math.clamp(rightVelocity, -1.0, 1.0);

    // Square the inputs (while preserving the sign) to increase fine control
    // while permitting full power.
    if (squareInputs) {
      leftVelocity = MathUtil.copyDirectionPow(leftVelocity, 2);
      rightVelocity = MathUtil.copyDirectionPow(rightVelocity, 2);
    }

    return new WheelVelocities(leftVelocity, rightVelocity);
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
    builder.addDoubleProperty("Left Motor Velocity", () -> m_leftOutput, m_leftMotor);
    builder.addDoubleProperty("Right Motor Velocity", () -> m_rightOutput, m_rightMotor);
  }
}
