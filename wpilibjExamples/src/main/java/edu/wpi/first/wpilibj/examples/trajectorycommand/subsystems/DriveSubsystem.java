// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.trajectorycommand.subsystems;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.controller.RamseteController;
import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.DifferentialDriveOdometry;
import edu.wpi.first.math.kinematics.DifferentialDriveWheelSpeeds;
import edu.wpi.first.math.trajectory.Trajectory;
import edu.wpi.first.wpilibj.ADXRS450_Gyro;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.examples.trajectorycommand.Constants.AutoConstants;
import edu.wpi.first.wpilibj.examples.trajectorycommand.Constants.DriveConstants;
import edu.wpi.first.wpilibj.interfaces.Gyro;
import edu.wpi.first.wpilibj.motorcontrol.MotorControllerGroup;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.TrajectoryCommand;

public class DriveSubsystem extends SubsystemBase {
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
  private final Gyro m_gyro = new ADXRS450_Gyro();

  // Odometry class for tracking robot pose
  private final DifferentialDriveOdometry m_odometry;
  private final RamseteController m_ramseteController =
      new RamseteController(AutoConstants.kRamseteB, AutoConstants.kRamseteZeta);
  private final SimpleMotorFeedforward m_feedforward =
      new SimpleMotorFeedforward(
          DriveConstants.ksVolts,
          DriveConstants.kvVoltSecondsPerMeter,
          DriveConstants.kaVoltSecondsSquaredPerMeter);
  private final PIDController m_leftController = new PIDController(DriveConstants.kPDriveVel, 0, 0);
  private final PIDController m_rightController =
      new PIDController(DriveConstants.kPDriveVel, 0, 0);

  // Track previous target velocities for feedforward calculation
  private DifferentialDriveWheelSpeeds m_previousSpeeds = new DifferentialDriveWheelSpeeds();

  /** Creates a new DriveSubsystem. */
  public DriveSubsystem() {
    // Sets the distance per pulse for the encoders
    m_leftEncoder.setDistancePerPulse(DriveConstants.kEncoderDistancePerPulse);
    m_rightEncoder.setDistancePerPulse(DriveConstants.kEncoderDistancePerPulse);

    resetEncoders();
    m_odometry = new DifferentialDriveOdometry(m_gyro.getRotation2d());
  }

  @Override
  public void periodic() {
    // Update the odometry in the periodic block
    m_odometry.update(
        m_gyro.getRotation2d(), m_leftEncoder.getDistance(), m_rightEncoder.getDistance());
  }

  /**
   * Returns the currently-estimated pose of the robot.
   *
   * @return The pose.
   */
  public Pose2d getPose() {
    return m_odometry.getPoseMeters();
  }

  /**
   * Resets the odometry to the specified pose.
   *
   * @param pose The pose to which to set the odometry.
   */
  public void resetOdometry(Pose2d pose) {
    resetEncoders();
    m_odometry.resetPosition(pose, m_gyro.getRotation2d());
  }

  /**
   * Drives the robot using arcade controls.
   *
   * @param fwd The commanded forward movement
   * @param rot The commanded rotation
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
   * @return The average of the two encoder readings
   */
  public double getAverageEncoderDistance() {
    return (m_leftEncoder.getDistance() + m_rightEncoder.getDistance()) / 2.0;
  }

  /**
   * Sets the max output of the drive. Useful for scaling the drive to drive more slowly.
   *
   * @param maxOutput The maximum output to which the drive will be constrained
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
   * @return The robot's heading in degrees, from -180 to 180
   */
  public double getHeading() {
    return m_gyro.getRotation2d().getDegrees();
  }

  /**
   * Returns the turn rate of the robot.
   *
   * @return The turn rate of the robot, in degrees per second
   */
  public double getTurnRate() {
    return -m_gyro.getRate();
  }

  /**
   * Reaches the given target state. Used internally to follow the TrajectoryCommand's output.
   *
   * @param targetState The target state struct.
   */
  private void followState(Trajectory.State targetState) {
    // Calculate via Ramsete the speed vector needed to reach the target state
    ChassisSpeeds speedVector =
        m_ramseteController.calculate(m_odometry.getPoseMeters(), targetState);
    // Convert the vector to the separate velocities of each side of the drivetrain
    DifferentialDriveWheelSpeeds targetSpeeds =
        DriveConstants.kDriveKinematics.toWheelSpeeds(speedVector);

    // PID and Feedforward control
    // This can be replaced by calls to smart motor controller closed-loop control
    // functionality. For example:
    // mySmartMotorController.setSetpoint(leftMetersPerSecond, ControlType.Velocity);

    double leftFeedforward =
        m_feedforward.calculate(
            m_previousSpeeds.leftMetersPerSecond,
            targetSpeeds.leftMetersPerSecond,
            AutoConstants.kTimestepSeconds);

    double rightFeedforward =
        m_feedforward.calculate(
            m_previousSpeeds.rightMetersPerSecond,
            targetSpeeds.rightMetersPerSecond,
            AutoConstants.kTimestepSeconds);

    double leftOutput =
        leftFeedforward
            + m_leftController.calculate(m_leftEncoder.getRate(), targetSpeeds.leftMetersPerSecond);

    double rightOutput =
        rightFeedforward
            + m_rightController.calculate(
                m_rightEncoder.getRate(), targetSpeeds.rightMetersPerSecond);

    // Apply the voltages
    m_leftMotors.setVoltage(leftOutput);
    m_rightMotors.setVoltage(rightOutput);
    m_drive.feed();

    // Track previous speed setpoints
    m_previousSpeeds = targetSpeeds;
  }

  /**
   * Build a command group for the given trajectory.
   *
   * <p>The group consists of:
   *
   * <p>- resetting the odometry to the trajectory's initial pose
   *
   * <p>- following the trajectory
   *
   * <p>- stopping at the end of the path
   *
   * @param trajectory The path to follow
   * @return A command group that tracks the given trajectory
   */
  public Command buildTrajectoryGroup(Trajectory trajectory) {
    return new TrajectoryCommand(trajectory, this::followState, this)
        // Reset odometry to the starting pose of the trajectory.
        .beforeStarting(() -> resetOdometry(trajectory.getInitialPose()), this)
        // Stop at the end of the trajectory.
        .andThen(
            () -> {
              m_previousSpeeds = new DifferentialDriveWheelSpeeds();
              m_drive.stopMotor();
            },
            this);
  }
}
