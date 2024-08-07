// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.drive;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.motorcontrol.MotorController;
import java.util.function.DoubleConsumer;

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
 * <p>Inputs smaller then {@value edu.wpi.first.wpilibj.drive.RobotDriveBase#kDefaultDeadband} will
 * be set to 0, and larger values will be scaled so that the full range is still used. This deadband
 * value can be changed with {@link #setDeadband}.
 *
 * <p>{@link edu.wpi.first.wpilibj.MotorSafety} is enabled by default. The driveCartesian or
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
   * Wheel speeds for a mecanum drive.
   *
   * <p>Uses normalized voltage [-1.0..1.0].
   */
  @SuppressWarnings("MemberName")
  public static class WheelSpeeds {
    /** Front-left wheel speed. */
    public double frontLeft;

    /** Front-right wheel speed. */
    public double frontRight;

    /** Rear-left wheel speed. */
    public double rearLeft;

    /** Rear-right wheel speed. */
    public double rearRight;

    /** Constructs a WheelSpeeds with zeroes for all four speeds. */
    public WheelSpeeds() {}

    /**
     * Constructs a WheelSpeeds.
     *
     * @param frontLeft The front left speed [-1.0..1.0].
     * @param frontRight The front right speed [-1.0..1.0].
     * @param rearLeft The rear left speed [-1.0..1.0].
     * @param rearRight The rear right speed [-1.0..1.0].
     */
    public WheelSpeeds(double frontLeft, double frontRight, double rearLeft, double rearRight) {
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
        (double output) -> frontLeftMotor.set(output),
        (double output) -> rearLeftMotor.set(output),
        (double output) -> frontRightMotor.set(output),
        (double output) -> rearRightMotor.set(output));
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
    SendableRegistry.addLW(this, "MecanumDrive", instances);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
  }

  /**
   * Drive method for Mecanum platform.
   *
   * <p>Angles are measured counterclockwise from the positive X axis. The robot's speed is
   * independent of its angle or rotation rate.
   *
   * @param xSpeed The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param ySpeed The robot's speed along the Y axis [-1.0..1.0]. Left is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   */
  public void driveCartesian(double xSpeed, double ySpeed, double zRotation) {
    driveCartesian(xSpeed, ySpeed, zRotation, Rotation2d.kZero);
  }

  /**
   * Drive method for Mecanum platform.
   *
   * <p>Angles are measured counterclockwise from the positive X axis. The robot's speed is
   * independent of its angle or rotation rate.
   *
   * @param xSpeed The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param ySpeed The robot's speed along the Y axis [-1.0..1.0]. Left is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   * @param gyroAngle The gyro heading around the Z axis. Use this to implement field-oriented
   *     controls.
   */
  public void driveCartesian(double xSpeed, double ySpeed, double zRotation, Rotation2d gyroAngle) {
    if (!m_reported) {
      HAL.report(
          tResourceType.kResourceType_RobotDrive, tInstances.kRobotDrive2_MecanumCartesian, 4);
      m_reported = true;
    }

    xSpeed = MathUtil.applyDeadband(xSpeed, m_deadband);
    ySpeed = MathUtil.applyDeadband(ySpeed, m_deadband);

    var speeds = driveCartesianIK(xSpeed, ySpeed, zRotation, gyroAngle);

    m_frontLeftOutput = speeds.frontLeft * m_maxOutput;
    m_rearLeftOutput = speeds.rearLeft * m_maxOutput;
    m_frontRightOutput = speeds.frontRight * m_maxOutput;
    m_rearRightOutput = speeds.rearRight * m_maxOutput;

    m_frontLeftMotor.accept(m_frontLeftOutput);
    m_frontRightMotor.accept(m_frontRightOutput);
    m_rearLeftMotor.accept(m_rearLeftOutput);
    m_rearRightMotor.accept(m_rearRightOutput);

    feed();
  }

  /**
   * Drive method for Mecanum platform.
   *
   * <p>Angles are measured counterclockwise from straight ahead. The speed at which the robot
   * drives (translation) is independent of its angle or rotation rate.
   *
   * @param magnitude The robot's speed at a given angle [-1.0..1.0]. Forward is positive.
   * @param angle The gyro heading around the Z axis at which the robot drives.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   */
  public void drivePolar(double magnitude, Rotation2d angle, double zRotation) {
    if (!m_reported) {
      HAL.report(tResourceType.kResourceType_RobotDrive, tInstances.kRobotDrive2_MecanumPolar, 4);
      m_reported = true;
    }

    driveCartesian(
        magnitude * angle.getCos(), magnitude * angle.getSin(), zRotation, Rotation2d.kZero);
  }

  /**
   * Cartesian inverse kinematics for Mecanum platform.
   *
   * <p>Angles are measured counterclockwise from the positive X axis. The robot's speed is
   * independent of its angle or rotation rate.
   *
   * @param xSpeed The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param ySpeed The robot's speed along the Y axis [-1.0..1.0]. Left is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   * @return Wheel speeds [-1.0..1.0].
   */
  public static WheelSpeeds driveCartesianIK(double xSpeed, double ySpeed, double zRotation) {
    return driveCartesianIK(xSpeed, ySpeed, zRotation, Rotation2d.kZero);
  }

  /**
   * Cartesian inverse kinematics for Mecanum platform.
   *
   * <p>Angles are measured clockwise from the positive X axis. The robot's speed is independent of
   * its angle or rotation rate.
   *
   * @param xSpeed The robot's speed along the X axis [-1.0..1.0]. Forward is positive.
   * @param ySpeed The robot's speed along the Y axis [-1.0..1.0]. Left is positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0]. Counterclockwise is
   *     positive.
   * @param gyroAngle The gyro heading around the Z axis. Use this to implement field-oriented
   *     controls.
   * @return Wheel speeds [-1.0..1.0].
   */
  public static WheelSpeeds driveCartesianIK(
      double xSpeed, double ySpeed, double zRotation, Rotation2d gyroAngle) {
    xSpeed = MathUtil.clamp(xSpeed, -1.0, 1.0);
    ySpeed = MathUtil.clamp(ySpeed, -1.0, 1.0);

    // Compensate for gyro angle.
    var input = new Translation2d(xSpeed, ySpeed).rotateBy(gyroAngle.unaryMinus());

    double[] wheelSpeeds = new double[4];
    wheelSpeeds[MotorType.kFrontLeft.value] = input.getX() + input.getY() + zRotation;
    wheelSpeeds[MotorType.kFrontRight.value] = input.getX() - input.getY() - zRotation;
    wheelSpeeds[MotorType.kRearLeft.value] = input.getX() - input.getY() + zRotation;
    wheelSpeeds[MotorType.kRearRight.value] = input.getX() + input.getY() - zRotation;

    normalize(wheelSpeeds);

    return new WheelSpeeds(
        wheelSpeeds[MotorType.kFrontLeft.value],
        wheelSpeeds[MotorType.kFrontRight.value],
        wheelSpeeds[MotorType.kRearLeft.value],
        wheelSpeeds[MotorType.kRearRight.value]);
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
    builder.setSafeState(this::stopMotor);
    builder.addDoubleProperty("Front Left Motor Speed", () -> m_frontLeftOutput, m_frontLeftMotor);
    builder.addDoubleProperty(
        "Front Right Motor Speed", () -> m_frontRightOutput, m_frontRightMotor);
    builder.addDoubleProperty("Rear Left Motor Speed", () -> m_rearLeftOutput, m_rearLeftMotor);
    builder.addDoubleProperty("Rear Right Motor Speed", () -> m_rearRightOutput, m_rearRightMotor);
  }
}
