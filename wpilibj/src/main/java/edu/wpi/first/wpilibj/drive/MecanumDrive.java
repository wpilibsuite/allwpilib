/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.drive;

import java.util.StringJoiner;

import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;
import edu.wpi.first.wpiutil.math.MathUtil;

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
 *
 * <p>This library uses the NED axes convention (North-East-Down as external reference in the world
 * frame): http://www.nuclearprojects.com/ins/images/axis_big.png.
 *
 * <p>The positive X axis points ahead, the positive Y axis points right, and the positive Z axis
 * points down. Rotations follow the right-hand rule, so clockwise rotation around the Z axis is
 * positive.
 *
 * <p>Inputs smaller then {@value edu.wpi.first.wpilibj.drive.RobotDriveBase#kDefaultDeadband} will
 * be set to 0, and larger values will be scaled so that the full range is still used. This
 * deadband value can be changed with {@link #setDeadband}.
 *
 * <p>RobotDrive porting guide:
 * <br>In MecanumDrive, the right side speed controllers are automatically inverted, while in
 * RobotDrive, no speed controllers are automatically inverted.
 * <br>{@link #driveCartesian(double, double, double, double)} is equivalent to
 * {@link edu.wpi.first.wpilibj.RobotDrive#mecanumDrive_Cartesian(double, double, double, double)}
 * if a deadband of 0 is used, and the ySpeed and gyroAngle values are inverted compared to
 * RobotDrive (eg driveCartesian(xSpeed, -ySpeed, zRotation, -gyroAngle).
 * <br>{@link #drivePolar(double, double, double)} is equivalent to
 * {@link edu.wpi.first.wpilibj.RobotDrive#mecanumDrive_Polar(double, double, double)} if a
 * deadband of 0 is used.
 */
public class MecanumDrive extends RobotDriveBase implements Sendable, AutoCloseable {
  private static int instances;

  private final SpeedController m_frontLeftMotor;
  private final SpeedController m_rearLeftMotor;
  private final SpeedController m_frontRightMotor;
  private final SpeedController m_rearRightMotor;

  private double m_rightSideInvertMultiplier = -1.0;
  private boolean m_reported;

  /**
   * Construct a MecanumDrive.
   *
   * <p>If a motor needs to be inverted, do so before passing it in.
   */
  public MecanumDrive(SpeedController frontLeftMotor, SpeedController rearLeftMotor,
                      SpeedController frontRightMotor, SpeedController rearRightMotor) {
    verify(frontLeftMotor, rearLeftMotor, frontRightMotor, rearRightMotor);
    m_frontLeftMotor = frontLeftMotor;
    m_rearLeftMotor = rearLeftMotor;
    m_frontRightMotor = frontRightMotor;
    m_rearRightMotor = rearRightMotor;
    SendableRegistry.addChild(this, m_frontLeftMotor);
    SendableRegistry.addChild(this, m_rearLeftMotor);
    SendableRegistry.addChild(this, m_frontRightMotor);
    SendableRegistry.addChild(this, m_rearRightMotor);
    instances++;
    SendableRegistry.addLW(this, "MecanumDrive", instances);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
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
  private void verify(SpeedController frontLeft, SpeedController rearLeft,
                      SpeedController frontRight, SpeedController rearRightMotor) {
    if (frontLeft != null && rearLeft != null && frontRight != null && rearRightMotor != null) {
      return;
    }
    StringJoiner joiner = new StringJoiner(", ");
    if (frontLeft == null) {
      joiner.add("frontLeftMotor");
    }
    if (rearLeft == null) {
      joiner.add("rearLeftMotor");
    }
    if (frontRight == null) {
      joiner.add("frontRightMotor");
    }
    if (rearRightMotor == null) {
      joiner.add("rearRightMotor");
    }
    throw new NullPointerException(joiner.toString());
  }

  /**
   * Drive method for Mecanum platform.
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
   * Drive method for Mecanum platform.
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
      HAL.report(tResourceType.kResourceType_RobotDrive,
                 tInstances.kRobotDrive2_MecanumCartesian, 4);
      m_reported = true;
    }

    ySpeed = MathUtil.clamp(ySpeed, -1.0, 1.0);
    ySpeed = applyDeadband(ySpeed, m_deadband);

    xSpeed = MathUtil.clamp(xSpeed, -1.0, 1.0);
    xSpeed = applyDeadband(xSpeed, m_deadband);

    // Compensate for gyro angle.
    Vector2d input = new Vector2d(ySpeed, xSpeed);
    input.rotate(-gyroAngle);

    double[] wheelSpeeds = new double[4];
    wheelSpeeds[MotorType.kFrontLeft.value] = input.x + input.y + zRotation;
    wheelSpeeds[MotorType.kFrontRight.value] = -input.x + input.y - zRotation;
    wheelSpeeds[MotorType.kRearLeft.value] = -input.x + input.y + zRotation;
    wheelSpeeds[MotorType.kRearRight.value] = input.x + input.y - zRotation;

    normalize(wheelSpeeds);

    m_frontLeftMotor.set(wheelSpeeds[MotorType.kFrontLeft.value] * m_maxOutput);
    m_frontRightMotor.set(wheelSpeeds[MotorType.kFrontRight.value] * m_maxOutput
        * m_rightSideInvertMultiplier);
    m_rearLeftMotor.set(wheelSpeeds[MotorType.kRearLeft.value] * m_maxOutput);
    m_rearRightMotor.set(wheelSpeeds[MotorType.kRearRight.value] * m_maxOutput
        * m_rightSideInvertMultiplier);

    feed();
  }

  /**
   * Drive method for Mecanum platform.
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
      HAL.report(tResourceType.kResourceType_RobotDrive, tInstances.kRobotDrive2_MecanumPolar, 4);
      m_reported = true;
    }

    driveCartesian(magnitude * Math.sin(angle * (Math.PI / 180.0)),
                   magnitude * Math.cos(angle * (Math.PI / 180.0)), zRotation, 0.0);
  }

  /**
   * Gets if the power sent to the right side of the drivetrain is multiplied by -1.
   *
   * @return true if the right side is inverted
   */
  public boolean isRightSideInverted() {
    return m_rightSideInvertMultiplier == -1.0;
  }

  /**
   * Sets if the power sent to the right side of the drivetrain should be multiplied by -1.
   *
   * @param rightSideInverted true if right side power should be multiplied by -1
   */
  public void setRightSideInverted(boolean rightSideInverted) {
    m_rightSideInvertMultiplier = rightSideInverted ? -1.0 : 1.0;
  }

  @Override
  public void stopMotor() {
    m_frontLeftMotor.stopMotor();
    m_frontRightMotor.stopMotor();
    m_rearLeftMotor.stopMotor();
    m_rearRightMotor.stopMotor();
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
    builder.addDoubleProperty("Front Left Motor Speed",
        m_frontLeftMotor::get,
        m_frontLeftMotor::set);
    builder.addDoubleProperty("Front Right Motor Speed",
        () -> m_frontRightMotor.get() * m_rightSideInvertMultiplier,
        value -> m_frontRightMotor.set(value * m_rightSideInvertMultiplier));
    builder.addDoubleProperty("Rear Left Motor Speed",
        m_rearLeftMotor::get,
        m_rearLeftMotor::set);
    builder.addDoubleProperty("Rear Right Motor Speed",
        () -> m_rearRightMotor.get() * m_rightSideInvertMultiplier,
        value -> m_rearRightMotor.set(value * m_rightSideInvertMultiplier));
  }
}
