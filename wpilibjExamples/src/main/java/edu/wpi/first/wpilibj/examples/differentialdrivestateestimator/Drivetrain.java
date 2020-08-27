/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.differentialdrivestateestimator;

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.PWMVictorSPX;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.SpeedControllerGroup;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.controller.LinearPlantInversionFeedforward;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.estimator.DifferentialDriveStateEstimator;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveWheelSpeeds;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N2;

import edu.wpi.first.wpilibj.examples.swervesdriveposeestimator.ExampleGlobalMeasurementSensor;

/**
 * Represents a differential drive style drivetrain.
 */
public class Drivetrain {
  public static final double kMaxSpeed = 3.0; // meters per second
  public static final double kMaxAngularSpeed = 2 * Math.PI; // one rotation per second

  private static final double kTrackWidth = 0.381 * 2; // meters
  private static final double kWheelRadius = 0.0508; // meters
  private static final int kEncoderResolution = 4096;

  private final SpeedController m_leftLeader = new PWMVictorSPX(1);
  private final SpeedController m_leftFollower = new PWMVictorSPX(2);
  private final SpeedController m_rightLeader = new PWMVictorSPX(3);
  private final SpeedController m_rightFollower = new PWMVictorSPX(4);

  private final Encoder m_leftEncoder = new Encoder(0, 1);
  private final Encoder m_rightEncoder = new Encoder(2, 3);

  private final SpeedControllerGroup m_leftGroup
      = new SpeedControllerGroup(m_leftLeader, m_leftFollower);
  private final SpeedControllerGroup m_rightGroup
      = new SpeedControllerGroup(m_rightLeader, m_rightFollower);

  private final AnalogGyro m_gyro = new AnalogGyro(0);

  private final PIDController m_leftPIDController = new PIDController(1, 0, 0);
  private final PIDController m_rightPIDController = new PIDController(1, 0, 0);

  private final DifferentialDriveKinematics m_kinematics
      = new DifferentialDriveKinematics(kTrackWidth);

  // Gains are for example purposes only - must be determined for your own robot!
  // You can determine this using frc-characterization.
  private final LinearSystem<N2, N2, N2> m_drivetrainSystem =
      LinearSystemId.identifyDrivetrainSystem(
      3, 4, 1, 2);

  /* Here we use DifferentialDriveStateEstimator so that we can fuse odometry readings. The
  numbers used below are robot specific, and should be tuned. */
  private final DifferentialDriveStateEstimator m_poseEstimator =
      new DifferentialDriveStateEstimator(
      m_drivetrainSystem,
      new Matrix<>(Nat.N10(), Nat.N1()),
      new MatBuilder<>(Nat.N10(), Nat.N1()).fill(
          0.002, 0.002, 0.0001, 1.5, 1.5, 0.5, 0.5, 10.0, 10.0, 2.0),
      new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.0001, 0.005, 0.005),
      new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.5, 0.5, 0.5),
      m_kinematics);

  // we use LinearPlantInversionFeedforward here to estimate feedforward voltages.
  private final LinearPlantInversionFeedforward<N2, N2, N2> m_feedforward =
      new LinearPlantInversionFeedforward<>(m_drivetrainSystem, 0.020);

  /**
   * Constructs a differential drive object.
   * Sets the encoder distance per pulse and resets the gyro.
   */
  public Drivetrain() {
    m_gyro.reset();

    // Set the distance per pulse for the drive encoders. We can simply use the
    // distance traveled for one rotation of the wheel divided by the encoder
    // resolution.
    m_leftEncoder.setDistancePerPulse(2 * Math.PI * kWheelRadius / kEncoderResolution);
    m_rightEncoder.setDistancePerPulse(2 * Math.PI * kWheelRadius / kEncoderResolution);

    m_leftEncoder.reset();
    m_rightEncoder.reset();
  }

  /**
   * Sets the desired wheel speeds.
   *
   * @param speeds The desired wheel speeds.
   */
  public void setSpeeds(DifferentialDriveWheelSpeeds speeds) {
    final var feedforward = m_feedforward.calculate(
        VecBuilder.fill(speeds.leftMetersPerSecond, speeds.rightMetersPerSecond));

    final double leftFeedforward = feedforward.get(0, 0);
    final double rightFeedforward = feedforward.get(1, 0);

    final double leftOutput = m_leftPIDController.calculate(m_leftEncoder.getRate(),
        speeds.leftMetersPerSecond);
    final double rightOutput = m_rightPIDController.calculate(m_rightEncoder.getRate(),
        speeds.rightMetersPerSecond);
    m_leftGroup.setVoltage(leftOutput + leftFeedforward);
    m_rightGroup.setVoltage(rightOutput + rightFeedforward);
  }

  /**
   * Drives the robot with the given linear velocity and angular velocity.
   *
   * @param xSpeed Linear velocity in m/s.
   * @param rot    Angular velocity in rad/s.
   */
  @SuppressWarnings("ParameterName")
  public void drive(double xSpeed, double rot) {
    var wheelSpeeds = m_kinematics.toWheelSpeeds(new ChassisSpeeds(xSpeed, 0.0, rot));
    setSpeeds(wheelSpeeds);
  }

  /**
   * Updates the field-relative position.
   */
  public void updateOdometry() {
    m_poseEstimator.update(m_gyro.getRotation2d().getRadians(),
        m_leftEncoder.getDistance(), m_rightEncoder.getDistance(),
        VecBuilder.fill(
            m_leftLeader.get() * RobotController.getBatteryVoltage(),
            m_rightLeader.get() * RobotController.getBatteryVoltage()
        ));

    // Also apply vision measurements. We use 0.3 seconds in the past as an example -- on
    // a real robot, this must be calculated based either on latency or timestamps.
    m_poseEstimator.applyPastGlobalMeasurement(
        ExampleGlobalMeasurementSensor.getEstimatedGlobalPose(
            m_poseEstimator.getEstimatedPosition()),
        Timer.getFPGATimestamp() - 0.3);
  }
}
