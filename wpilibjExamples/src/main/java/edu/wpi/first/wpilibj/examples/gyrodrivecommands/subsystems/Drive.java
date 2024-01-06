// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gyrodrivecommands.subsystems;

import static edu.wpi.first.wpilibj.examples.gyrodrivecommands.Constants.DriveConstants.*;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.controller.ProfiledPIDController;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.trajectory.TrapezoidProfile;
import edu.wpi.first.wpilibj.ADXRS450_Gyro;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.examples.gyrodrivecommands.Constants.DriveConstants;
import edu.wpi.first.wpilibj.motorcontrol.MotorControllerGroup;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import java.util.function.DoubleSupplier;

public class Drive extends SubsystemBase {
  // The motors on the left side of the drive.
  private final MotorControllerGroup m_leftMotors =
      new MotorControllerGroup(
          new PWMSparkMax(DriveConstants.kLeftMotor1Port),
          new PWMSparkMax(DriveConstants.kLeftMotor2Port));

  // The motors on the right side of the drive.
  private final MotorControllerGroup m_rightMotors =
      new MotorControllerGroup(
          new PWMSparkMax(DriveConstants.kRightMotor1Port),
          new PWMSparkMax(DriveConstants.kRightMotor2Port));

  // The robot's drive
  private final DifferentialDrive m_drive = new DifferentialDrive(m_leftMotors, m_rightMotors);

  // The left-side drive encoder
  private final Encoder m_leftEncoder =
      new Encoder(
          DriveConstants.kLeftEncoderPorts[0],
          DriveConstants.kLeftEncoderPorts[1],
          DriveConstants.kLeftEncoderReversed);

  // The right-side drive encoder
  private final Encoder m_rightEncoder =
      new Encoder(
          DriveConstants.kRightEncoderPorts[0],
          DriveConstants.kRightEncoderPorts[1],
          DriveConstants.kRightEncoderReversed);

  // The gyro sensor
  private final ADXRS450_Gyro m_gyro = new ADXRS450_Gyro();

  /** Creates a new DriveSubsystem. */
  public Drive() {
    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    m_rightMotors.setInverted(true);

    // Sets the distance per pulse for the encoders
    m_leftEncoder.setDistancePerPulse(DriveConstants.kEncoderDistancePerPulse);
    m_rightEncoder.setDistancePerPulse(DriveConstants.kEncoderDistancePerPulse);
  }

  /**
   * Drives the robot using arcade controls.
   *
   * @param fwd the commanded forward movement
   * @param rot the commanded rotation
   */
  public void arcadeDrive(double fwd, double rot) {
    m_drive.arcadeDrive(fwd, rot);
  }

  /** Resets the drive encoders to currently read a position of 0. */
  public void resetEncoders() {
    m_leftEncoder.reset();
    m_rightEncoder.reset();
  }

  /**
   * Gets the average distance of the two encoders.
   *
   * @return the average of the two encoder readings
   */
  public double getAverageEncoderDistance() {
    return (m_leftEncoder.getDistance() + m_rightEncoder.getDistance()) / 2.0;
  }

  /**
   * Gets the left drive encoder.
   *
   * @return the left drive encoder
   */
  public Encoder getLeftEncoder() {
    return m_leftEncoder;
  }

  /**
   * Gets the right drive encoder.
   *
   * @return the right drive encoder
   */
  public Encoder getRightEncoder() {
    return m_rightEncoder;
  }

  /**
   * Sets the max output of the drive. Useful for scaling the drive to drive more slowly.
   *
   * @param maxOutput the maximum output to which the drive will be constrained
   */
  public void setMaxOutput(double maxOutput) {
    m_drive.setMaxOutput(maxOutput);
  }

  /** Zeroes the heading of the robot. */
  public void zeroHeading() {
    m_gyro.reset();
  }

  /**
   * Returns the heading of the robot.
   *
   * @return the robot's heading as a Rotation2d.
   */
  public Rotation2d getHeading() {
    return m_gyro.getRotation2d().times(DriveConstants.kGyroReversed ? -1.0 : 1.0);
  }

  /**
   * Returns the turn rate of the robot.
   *
   * @return The turn rate of the robot, in degrees per second
   */
  public double getTurnRate() {
    return m_gyro.getRate() * (DriveConstants.kGyroReversed ? -1.0 : 1.0);
  }

  /**
   * A command that turns the robot to the specified angle.
   *
   * @param targetAngle The angle to turn to.
   * @return A command to turn the robot to the desired angle.
   */
  @SuppressWarnings("resource")
  public Command turnToAngle(Rotation2d targetAngle) {
    PIDController controller = new PIDController(kTurnP, kTurnI, kTurnD);
    controller.enableContinuousInput(-180, 180);
    controller.setTolerance(kTurnToleranceDeg, kTurnRateToleranceDegPerS);
    return run(() ->
            arcadeDrive(
                0, controller.calculate(getHeading().getDegrees(), targetAngle.getDegrees())))
        .until(controller::atSetpoint);
  }

  /**
   * A command that smoothly turns the robot to the specified angle.
   *
   * @param targetAngle The angle to turn to.
   * @return A command to turn the robot to the desired angle.
   */
  public Command turnToAngleProfiled(Rotation2d targetAngle) {
    ProfiledPIDController controller =
        new ProfiledPIDController(
            kTurnP,
            kTurnI,
            kTurnD,
            new TrapezoidProfile.Constraints(
                kMaxTurnRateDegPerS, kMaxTurnAccelerationDegPerSSquared));
    controller.enableContinuousInput(-180, 180);
    controller.setTolerance(kTurnToleranceDeg, kTurnRateToleranceDegPerS);
    return run(() ->
            arcadeDrive(
                0, controller.calculate(getHeading().getDegrees(), targetAngle.getDegrees())))
        .until(controller::atSetpoint);
  }

  /**
   * A command to stabalize the robot to drive straight at desired speeds.
   *
   * @param speed A supplier of the desired speed for the robot.
   * @return A command to move in a straight line at the desired speed.
   */
  @SuppressWarnings("resource")
  public Command stabilize(DoubleSupplier speed) {
    // setpoint defaults to 0
    PIDController controller = new PIDController(kStabilizationP, kStabilizationI, kStabilizationD);
    return run(() -> arcadeDrive(speed.getAsDouble(), controller.calculate(getTurnRate())));
  }
}
