// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.mecanumdriveposeestimator;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.math.estimator.MecanumDrivePoseEstimator;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.kinematics.MecanumDriveWheelPositions;
import edu.wpi.first.math.kinematics.MecanumDriveWheelSpeeds;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

/** Represents a mecanum drive style drivetrain. */
public class Drivetrain {
  public static final double kMaxSpeed = 3.0; // 3 meters per second
  public static final double kMaxAngularSpeed = Math.PI; // 1/2 rotation per second

  private final PWMSparkMax m_frontLeftMotor = new PWMSparkMax(1);
  private final PWMSparkMax m_frontRightMotor = new PWMSparkMax(2);
  private final PWMSparkMax m_backLeftMotor = new PWMSparkMax(3);
  private final PWMSparkMax m_backRightMotor = new PWMSparkMax(4);

  private final Encoder m_frontLeftEncoder = new Encoder(0, 1);
  private final Encoder m_frontRightEncoder = new Encoder(2, 3);
  private final Encoder m_backLeftEncoder = new Encoder(4, 5);
  private final Encoder m_backRightEncoder = new Encoder(6, 7);

  private final Translation2d m_frontLeftLocation = new Translation2d(0.381, 0.381);
  private final Translation2d m_frontRightLocation = new Translation2d(0.381, -0.381);
  private final Translation2d m_backLeftLocation = new Translation2d(-0.381, 0.381);
  private final Translation2d m_backRightLocation = new Translation2d(-0.381, -0.381);

  private final PIDController m_frontLeftPIDController = new PIDController(1, 0, 0);
  private final PIDController m_frontRightPIDController = new PIDController(1, 0, 0);
  private final PIDController m_backLeftPIDController = new PIDController(1, 0, 0);
  private final PIDController m_backRightPIDController = new PIDController(1, 0, 0);

  private final AnalogGyro m_gyro = new AnalogGyro(0);

  private final MecanumDriveKinematics m_kinematics =
      new MecanumDriveKinematics(
          m_frontLeftLocation, m_frontRightLocation, m_backLeftLocation, m_backRightLocation);

  /* Here we use MecanumDrivePoseEstimator so that we can fuse odometry readings. The numbers used
  below are robot specific, and should be tuned. */
  private final MecanumDrivePoseEstimator m_poseEstimator =
      new MecanumDrivePoseEstimator(
          m_kinematics,
          m_gyro.getRotation2d(),
          getCurrentDistances(),
          Pose2d.kZero,
          VecBuilder.fill(0.05, 0.05, Units.degreesToRadians(5)),
          VecBuilder.fill(0.5, 0.5, Units.degreesToRadians(30)));

  // Gains are for example purposes only - must be determined for your own robot!
  private final SimpleMotorFeedforward m_feedforward = new SimpleMotorFeedforward(1, 3);

  /** Constructs a MecanumDrive and resets the gyro. */
  public Drivetrain() {
    m_gyro.reset();
    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    m_frontRightMotor.setInverted(true);
    m_backRightMotor.setInverted(true);
  }

  /**
   * Returns the current state of the drivetrain.
   *
   * @return The current state of the drivetrain.
   */
  public MecanumDriveWheelSpeeds getCurrentState() {
    return new MecanumDriveWheelSpeeds(
        m_frontLeftEncoder.getRate(),
        m_frontRightEncoder.getRate(),
        m_backLeftEncoder.getRate(),
        m_backRightEncoder.getRate());
  }

  /**
   * Returns the current distances measured by the drivetrain.
   *
   * @return The current distances measured by the drivetrain.
   */
  public MecanumDriveWheelPositions getCurrentDistances() {
    return new MecanumDriveWheelPositions(
        m_frontLeftEncoder.getDistance(),
        m_frontRightEncoder.getDistance(),
        m_backLeftEncoder.getDistance(),
        m_backRightEncoder.getDistance());
  }

  /**
   * Set the desired speeds for each wheel.
   *
   * @param speeds The desired wheel speeds.
   */
  public void setSpeeds(MecanumDriveWheelSpeeds speeds) {
    final double frontLeftFeedforward = m_feedforward.calculate(speeds.frontLeftMetersPerSecond);
    final double frontRightFeedforward = m_feedforward.calculate(speeds.frontRightMetersPerSecond);
    final double backLeftFeedforward = m_feedforward.calculate(speeds.rearLeftMetersPerSecond);
    final double backRightFeedforward = m_feedforward.calculate(speeds.rearRightMetersPerSecond);

    final double frontLeftOutput =
        m_frontLeftPIDController.calculate(
            m_frontLeftEncoder.getRate(), speeds.frontLeftMetersPerSecond);
    final double frontRightOutput =
        m_frontRightPIDController.calculate(
            m_frontRightEncoder.getRate(), speeds.frontRightMetersPerSecond);
    final double backLeftOutput =
        m_backLeftPIDController.calculate(
            m_backLeftEncoder.getRate(), speeds.rearLeftMetersPerSecond);
    final double backRightOutput =
        m_backRightPIDController.calculate(
            m_backRightEncoder.getRate(), speeds.rearRightMetersPerSecond);

    m_frontLeftMotor.setVoltage(frontLeftOutput + frontLeftFeedforward);
    m_frontRightMotor.setVoltage(frontRightOutput + frontRightFeedforward);
    m_backLeftMotor.setVoltage(backLeftOutput + backLeftFeedforward);
    m_backRightMotor.setVoltage(backRightOutput + backRightFeedforward);
  }

  /**
   * Method to drive the robot using joystick info.
   *
   * @param xSpeed Speed of the robot in the x direction (forward).
   * @param ySpeed Speed of the robot in the y direction (sideways).
   * @param rot Angular rate of the robot.
   * @param fieldRelative Whether the provided x and y speeds are relative to the field.
   */
  public void drive(
      double xSpeed, double ySpeed, double rot, boolean fieldRelative, double periodSeconds) {
    var mecanumDriveWheelSpeeds =
        m_kinematics.toWheelSpeeds(
            ChassisSpeeds.discretize(
                fieldRelative
                    ? ChassisSpeeds.fromFieldRelativeSpeeds(
                        xSpeed, ySpeed, rot, m_poseEstimator.getEstimatedPosition().getRotation())
                    : new ChassisSpeeds(xSpeed, ySpeed, rot),
                periodSeconds));
    mecanumDriveWheelSpeeds.desaturate(kMaxSpeed);
    setSpeeds(mecanumDriveWheelSpeeds);
  }

  /** Updates the field relative position of the robot. */
  public void updateOdometry() {
    m_poseEstimator.update(m_gyro.getRotation2d(), getCurrentDistances());

    // Also apply vision measurements. We use 0.3 seconds in the past as an example -- on
    // a real robot, this must be calculated based either on latency or timestamps.
    m_poseEstimator.addVisionMeasurement(
        ExampleGlobalMeasurementSensor.getEstimatedGlobalPose(
            m_poseEstimator.getEstimatedPosition()),
        Timer.getTimestamp() - 0.3);
  }
}
