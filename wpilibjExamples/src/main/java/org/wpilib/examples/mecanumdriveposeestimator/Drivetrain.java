// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.mecanumdriveposeestimator;

import org.wpilib.hardware.imu.OnboardIMU;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.controller.SimpleMotorFeedforward;
import org.wpilib.math.estimator.MecanumDrivePoseEstimator;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.kinematics.MecanumDriveKinematics;
import org.wpilib.math.kinematics.MecanumDriveWheelPositions;
import org.wpilib.math.kinematics.MecanumDriveWheelVelocities;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.util.Units;
import org.wpilib.system.Timer;

/** Represents a mecanum drive style drivetrain. */
public class Drivetrain {
  public static final double kMaxVelocity = 3.0; // 3 meters per second
  public static final double kMaxAngularVelocity = Math.PI; // 1/2 rotation per second

  private final PWMSparkMax frontLeftMotor = new PWMSparkMax(1);
  private final PWMSparkMax frontRightMotor = new PWMSparkMax(2);
  private final PWMSparkMax backLeftMotor = new PWMSparkMax(3);
  private final PWMSparkMax backRightMotor = new PWMSparkMax(4);

  private final Encoder frontLeftEncoder = new Encoder(0, 1);
  private final Encoder frontRightEncoder = new Encoder(2, 3);
  private final Encoder backLeftEncoder = new Encoder(4, 5);
  private final Encoder backRightEncoder = new Encoder(6, 7);

  private final Translation2d frontLeftLocation = new Translation2d(0.381, 0.381);
  private final Translation2d frontRightLocation = new Translation2d(0.381, -0.381);
  private final Translation2d backLeftLocation = new Translation2d(-0.381, 0.381);
  private final Translation2d backRightLocation = new Translation2d(-0.381, -0.381);

  private final PIDController frontLeftPIDController = new PIDController(1, 0, 0);
  private final PIDController frontRightPIDController = new PIDController(1, 0, 0);
  private final PIDController backLeftPIDController = new PIDController(1, 0, 0);
  private final PIDController backRightPIDController = new PIDController(1, 0, 0);

  private final OnboardIMU imu = new OnboardIMU(OnboardIMU.MountOrientation.FLAT);

  private final MecanumDriveKinematics kinematics =
      new MecanumDriveKinematics(
          frontLeftLocation, frontRightLocation, backLeftLocation, backRightLocation);

  /* Here we use MecanumDrivePoseEstimator so that we can fuse odometry readings. The numbers used
  below are robot specific, and should be tuned. */
  private final MecanumDrivePoseEstimator poseEstimator =
      new MecanumDrivePoseEstimator(
          kinematics,
          imu.getRotation2d(),
          getCurrentWheelDistances(),
          Pose2d.kZero,
          VecBuilder.fill(0.05, 0.05, Units.degreesToRadians(5)),
          VecBuilder.fill(0.5, 0.5, Units.degreesToRadians(30)));

  // Gains are for example purposes only - must be determined for your own robot!
  private final SimpleMotorFeedforward feedforward = new SimpleMotorFeedforward(1, 3);

  /** Constructs a MecanumDrive and resets the gyro. */
  public Drivetrain() {
    imu.resetYaw();
    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    frontRightMotor.setInverted(true);
    backRightMotor.setInverted(true);
  }

  /**
   * Returns the current wheel distances measured by the drivetrain.
   *
   * @return The current wheel distances measured by the drivetrain.
   */
  public MecanumDriveWheelPositions getCurrentWheelDistances() {
    return new MecanumDriveWheelPositions(
        frontLeftEncoder.getDistance(),
        frontRightEncoder.getDistance(),
        backLeftEncoder.getDistance(),
        backRightEncoder.getDistance());
  }

  /**
   * Returns the current wheel velocities of the drivetrain.
   *
   * @return The current wheel velocities of the drivetrain.
   */
  public MecanumDriveWheelVelocities getCurrentWheelVelocities() {
    return new MecanumDriveWheelVelocities(
        frontLeftEncoder.getRate(),
        frontRightEncoder.getRate(),
        backLeftEncoder.getRate(),
        backRightEncoder.getRate());
  }

  /**
   * Set the desired velocities for each wheel.
   *
   * @param velocities The desired wheel velocities.
   */
  public void setVelocities(MecanumDriveWheelVelocities velocities) {
    final double frontLeftFeedforward = feedforward.calculate(velocities.frontLeft);
    final double frontRightFeedforward = feedforward.calculate(velocities.frontRight);
    final double backLeftFeedforward = feedforward.calculate(velocities.rearLeft);
    final double backRightFeedforward = feedforward.calculate(velocities.rearRight);

    final double frontLeftOutput =
        frontLeftPIDController.calculate(frontLeftEncoder.getRate(), velocities.frontLeft);
    final double frontRightOutput =
        frontRightPIDController.calculate(frontRightEncoder.getRate(), velocities.frontRight);
    final double backLeftOutput =
        backLeftPIDController.calculate(backLeftEncoder.getRate(), velocities.rearLeft);
    final double backRightOutput =
        backRightPIDController.calculate(backRightEncoder.getRate(), velocities.rearRight);

    frontLeftMotor.setVoltage(frontLeftOutput + frontLeftFeedforward);
    frontRightMotor.setVoltage(frontRightOutput + frontRightFeedforward);
    backLeftMotor.setVoltage(backLeftOutput + backLeftFeedforward);
    backRightMotor.setVoltage(backRightOutput + backRightFeedforward);
  }

  /**
   * Method to drive the robot using joystick info.
   *
   * @param xVelocity Velocity of the robot in the x direction (forward).
   * @param yVelocity Velocity of the robot in the y direction (sideways).
   * @param rot Angular rate of the robot.
   * @param fieldRelative Whether the provided x and y velocities are relative to the field.
   */
  public void drive(
      double xVelocity, double yVelocity, double rot, boolean fieldRelative, double period) {
    var chassisVelocities = new ChassisVelocities(xVelocity, yVelocity, rot);
    if (fieldRelative) {
      chassisVelocities =
          chassisVelocities.toRobotRelative(poseEstimator.getEstimatedPosition().getRotation());
    }
    setVelocities(
        kinematics
            .toWheelVelocities(chassisVelocities.discretize(period))
            .desaturate(kMaxVelocity));
  }

  /** Updates the field relative position of the robot. */
  public void updateOdometry() {
    poseEstimator.update(imu.getRotation2d(), getCurrentWheelDistances());

    // Also apply vision measurements. We use 0.3 seconds in the past as an example -- on
    // a real robot, this must be calculated based either on latency or timestamps.
    poseEstimator.addVisionMeasurement(
        ExampleGlobalMeasurementSensor.getEstimatedGlobalPose(poseEstimator.getEstimatedPosition()),
        Timer.getTimestamp() - 0.3);
  }
}
