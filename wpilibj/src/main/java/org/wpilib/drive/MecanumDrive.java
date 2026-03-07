// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.drive;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.function.DoubleConsumer;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.motor.MotorController;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.util.MathUtil;
import org.wpilib.util.sendable.Sendable;
import org.wpilib.util.sendable.SendableBuilder;
import org.wpilib.util.sendable.SendableRegistry;

/**
 * A class for driving Mecanum drive platforms.
 *
 * <p>Mecanum drives are rectangular with one wheel on each corner. Each wheel has rollers toed in
 * 45 degrees toward the front or back. When looking at the wheels from the top, the roller axles
 * should form an X across the robot. Each drive() function provides different inverse kinematic
 * relations for a Mecanum drive robot.
 *
 * <p>Drive base diagram:
 *
 * <pre>
 * \\_______/
 * \\ |   | /
 *   |   |
 * /_|___|_\\
 * /       \\
 * </pre>
 *
 * <p>Each drive() function provides different inverse kinematic relations for a Mecanum drive
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
 * <p>{@link org.wpilib.hardware.motor.MotorSafety} is enabled by default. The driveCartesian or
 * drivePolar methods should be called periodically to avoid Motor Safety timeouts.
 */
public class MecanumDrive extends RobotDriveBase implements Sendable, AutoCloseable {
  private static int instances;

  private final DoubleConsumer m_frontLeftMotor;
  private final DoubleConsumer m_rearLeftMotor;
  private final DoubleConsumer m_frontRightMotor;
  private final DoubleConsumer m_rearRightMotor;

  // Used for Sendable property getters
  private double m_frontLeftOutput;
  private double m_rearLeftOutput;
  private double m_frontRightOutput;
  private double m_rearRightOutput;

  private boolean m_reported;

  /**
   * Wheel velocities for a mecanum drive.
   *
   * <p>Uses normalized voltage [-1.0..1.0].
   */
  @SuppressWarnings("MemberName")
  public static class WheelVelocities {
    /** Front-left wheel velocity. */
    public double frontLeft;

    /** Front-right wheel velocity. */
    public double frontRight;

    /** Rear-left wheel velocity. */
    public double rearLeft;

    /** Rear-right wheel velocity. */
    public double rearRight;

    /** Constructs a WheelVelocities with zeroes for all four velocities. */
    public WheelVelocities() {}

    /**
     * Constructs a WheelVelocities.
     *
     * @param frontLeft The front left velocity [-1.0..1.0].
     * @param frontRight The front right velocity [-1.0..1.0].
     * @param rearLeft The rear left velocity [-1.0..1.0].
     * @param rearRight The rear right velocity [-1.0..1.0].
     */
    public WheelVelocities(double frontLeft, double frontRight, double rearLeft, double rearRight) {
      this.frontLeft = frontLeft;
      this.frontRight = frontRight;
      this.rearLeft = rearLeft;
      this.rearRight = rearRight;
    }
  }

  /**
   * Construct a MecanumDrive.
   *
   * <p>If a motor needs to be inverted, do so before passing it in.
   *
   * @param frontLeftMotor The motor on the front-left corner.
   * @param rearLeftMotor The motor on the rear-left corner.
   * @param frontRightMotor The motor on the front-right corner.
   * @param rearRightMotor The motor on the rear-right corner.
   */
  @SuppressWarnings({"removal", "this-escape"})
  public MecanumDrive(
      MotorController frontLeftMotor,
      MotorController rearLeftMotor,
      MotorController frontRightMotor,
      MotorController rearRightMotor) {
    this(
        (double output) -> frontLeftMotor.setDutyCycle(output),
        (double output) -> rearLeftMotor.setDutyCycle(output),
        (double output) -> frontRightMotor.setDutyCycle(output),
        (double output) -> rearRightMotor.setDutyCycle(output));
    SendableRegistry.addChild(this, frontLeftMotor);
    SendableRegistry.addChild(this, rearLeftMotor);
    SendableRegistry.addChild(this, frontRightMotor);
    SendableRegistry.addChild(this, rearRightMotor);
  }

  /**
   * Construct a MecanumDrive.
   *
   * <p>If a motor needs to be inverted, do so before passing it in.
   *
   * @param frontLeftMotor The setter for the motor on the front-left corner.
   * @param rearLeftMotor The setter for the motor on the rear-left corner.
   * @param frontRightMotor The setter for the motor on the front-right corner.
   * @param rearRightMotor The setter for the motor on the rear-right corner.
   */
  @SuppressWarnings("this-escape")
  public MecanumDrive(
      DoubleConsumer frontLeftMotor,
      DoubleConsumer rearLeftMotor,
      DoubleConsumer frontRightMotor,
      DoubleConsumer rearRightMotor) {
    requireNonNullParam(frontLeftMotor, "frontLeftMotor", "MecanumDrive");
    requireNonNullParam(rearLeftMotor, "rearLeftMotor", "MecanumDrive");
    requireNonNullParam(frontRightMotor, "frontRightMotor", "MecanumDrive");
    requireNonNullParam(rearRightMotor, "rearRightMotor", "MecanumDrive");

    m_frontLeftMotor = frontLeftMotor;
    m_rearLeftMotor = rearLeftMotor;
    m_frontRightMotor = frontRightMotor;
    m_rearRightMotor = rearRightMotor;
    instances++;
    SendableRegistry.add(this, "MecanumDrive", instances);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
  }

  /**
   * Drive method for Mecanum platform.
   *
   * <p>Angles are measured counterclockwise from the positive X axis. The robot's velocity is
   * independent of its angle or rotation rate.
   *
   * @param xVelocity The robot's velocity along the X axis [-1.0..1.0]. Forward is positive.
   * @param yVelocity The robot's velocity along the Y axis [-1.0..1.0]. Left is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   */
  public void driveCartesian(double xVelocity, double yVelocity, double zRotation) {
    driveCartesian(xVelocity, yVelocity, zRotation, Rotation2d.kZero);
  }

  /**
   * Drive method for Mecanum platform.
   *
   * <p>Angles are measured counterclockwise from the positive X axis. The robot's velocity is
   * independent of its angle or rotation rate.
   *
   * @param xVelocity The robot's velocity along the X axis [-1.0..1.0]. Forward is positive.
   * @param yVelocity The robot's velocity along the Y axis [-1.0..1.0]. Left is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   * @param gyroAngle The gyro heading around the Z axis. Use this to implement field-oriented
   *     controls.
   */
  public void driveCartesian(
      double xVelocity, double yVelocity, double zRotation, Rotation2d gyroAngle) {
    if (!m_reported) {
      HAL.reportUsage("RobotDrive", "MecanumCartesian");
      m_reported = true;
    }

    xVelocity = MathUtil.applyDeadband(xVelocity, m_deadband);
    yVelocity = MathUtil.applyDeadband(yVelocity, m_deadband);

    var velocities = driveCartesianIK(xVelocity, yVelocity, zRotation, gyroAngle);

    m_frontLeftOutput = velocities.frontLeft * m_maxOutput;
    m_rearLeftOutput = velocities.rearLeft * m_maxOutput;
    m_frontRightOutput = velocities.frontRight * m_maxOutput;
    m_rearRightOutput = velocities.rearRight * m_maxOutput;

    m_frontLeftMotor.accept(m_frontLeftOutput);
    m_frontRightMotor.accept(m_frontRightOutput);
    m_rearLeftMotor.accept(m_rearLeftOutput);
    m_rearRightMotor.accept(m_rearRightOutput);

    feed();
  }

  /**
   * Drive method for Mecanum platform.
   *
   * <p>Angles are measured counterclockwise from straight ahead. The velocity at which the robot
   * drives (translation) is independent of its angle or rotation rate.
   *
   * @param magnitude The robot's velocity at a given angle [-1.0..1.0]. Forward is positive.
   * @param angle The gyro heading around the Z axis at which the robot drives.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   */
  public void drivePolar(double magnitude, Rotation2d angle, double zRotation) {
    if (!m_reported) {
      HAL.reportUsage("RobotDrive", "MecanumPolar");
      m_reported = true;
    }

    driveCartesian(
        magnitude * angle.getCos(), magnitude * angle.getSin(), zRotation, Rotation2d.kZero);
  }

  /**
   * Cartesian inverse kinematics for Mecanum platform.
   *
   * <p>Angles are measured counterclockwise from the positive X axis. The robot's velocity is
   * independent of its angle or rotation rate.
   *
   * @param xVelocity The robot's velocity along the X axis [-1.0..1.0]. Forward is positive.
   * @param yVelocity The robot's velocity along the Y axis [-1.0..1.0]. Left is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   * @return Wheel velocities [-1.0..1.0].
   */
  public static WheelVelocities driveCartesianIK(
      double xVelocity, double yVelocity, double zRotation) {
    return driveCartesianIK(xVelocity, yVelocity, zRotation, Rotation2d.kZero);
  }

  /**
   * Cartesian inverse kinematics for Mecanum platform.
   *
   * <p>Angles are measured clockwise from the positive X axis. The robot's velocity is independent
   * of its angle or rotation rate.
   *
   * @param xVelocity The robot's velocity along the X axis [-1.0..1.0]. Forward is positive.
   * @param yVelocity The robot's velocity along the Y axis [-1.0..1.0]. Left is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   * @param gyroAngle The gyro heading around the Z axis. Use this to implement field-oriented
   *     controls.
   * @return Wheel velocities [-1.0..1.0].
   */
  public static WheelVelocities driveCartesianIK(
      double xVelocity, double yVelocity, double zRotation, Rotation2d gyroAngle) {
    xVelocity = Math.clamp(xVelocity, -1.0, 1.0);
    yVelocity = Math.clamp(yVelocity, -1.0, 1.0);

    // Compensate for gyro angle.
    var input = new Translation2d(xVelocity, yVelocity).rotateBy(gyroAngle.unaryMinus());

    double[] wheelVelocities = new double[4];
    wheelVelocities[MotorType.kFrontLeft.value] = input.getX() + input.getY() + zRotation;
    wheelVelocities[MotorType.kFrontRight.value] = input.getX() - input.getY() - zRotation;
    wheelVelocities[MotorType.kRearLeft.value] = input.getX() - input.getY() + zRotation;
    wheelVelocities[MotorType.kRearRight.value] = input.getX() + input.getY() - zRotation;

    normalize(wheelVelocities);

    return new WheelVelocities(
        wheelVelocities[MotorType.kFrontLeft.value],
        wheelVelocities[MotorType.kFrontRight.value],
        wheelVelocities[MotorType.kRearLeft.value],
        wheelVelocities[MotorType.kRearRight.value]);
  }

  @Override
  public void stopMotor() {
    m_frontLeftOutput = 0.0;
    m_frontRightOutput = 0.0;
    m_rearLeftOutput = 0.0;
    m_rearRightOutput = 0.0;

    m_frontLeftMotor.accept(0.0);
    m_frontRightMotor.accept(0.0);
    m_rearLeftMotor.accept(0.0);
    m_rearRightMotor.accept(0.0);

    feed();
  }

  @Override
  public String getDescription() {
    return "MecanumDrive";
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("MecanumDrive");
    builder.setActuator(true);
    builder.addDoubleProperty(
        "Front Left Motor Velocity", () -> m_frontLeftOutput, m_frontLeftMotor);
    builder.addDoubleProperty(
        "Front Right Motor Velocity", () -> m_frontRightOutput, m_frontRightMotor);
    builder.addDoubleProperty("Rear Left Motor Velocity", () -> m_rearLeftOutput, m_rearLeftMotor);
    builder.addDoubleProperty(
        "Rear Right Motor Velocity", () -> m_rearRightOutput, m_rearRightMotor);
  }
}
