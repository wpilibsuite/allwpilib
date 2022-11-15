// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.interpolation.TimeInterpolatableBuffer;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.kinematics.MecanumDriveWheelPositions;
import edu.wpi.first.math.kinematics.MecanumDriveWheelSpeeds;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.numbers.N5;
import edu.wpi.first.math.numbers.N7;
import edu.wpi.first.util.WPIUtilJNI;
import java.util.function.BiConsumer;

/**
 * This class wraps an {@link UnscentedKalmanFilter Unscented Kalman Filter} to fuse
 * latency-compensated vision measurements with mecanum drive encoder velocity measurements. It will
 * correct for noisy measurements and encoder drift. It is intended to be an easy but more accurate
 * drop-in for {@link edu.wpi.first.math.kinematics.MecanumDriveOdometry}.
 *
 * <p>{@link MecanumDrivePoseEstimator#update} should be called every robot loop. If your loops are
 * faster or slower than the default of 20 ms, then you should change the nominal delta time using
 * the secondary constructor: {@link MecanumDrivePoseEstimator#MecanumDrivePoseEstimator(Rotation2d,
 * Pose2d, MecanumDriveWheelPositions, MecanumDriveKinematics, Matrix, Matrix, Matrix, double)}.
 *
 * <p>{@link MecanumDrivePoseEstimator#addVisionMeasurement} can be called as infrequently as you
 * want; if you never call it, then this class will behave mostly like regular encoder odometry.
 *
 * <p>The state-space system used internally has the following states (x), inputs (u), and outputs
 * (y):
 *
 * <p><strong> x = [x, y, theta, s_fl, s_fr, s_rl, s_rr]ᵀ </strong> in the field coordinate system
 * containing x position, y position, and heading, followed by the distance driven by the front
 * left, front right, rear left, and rear right wheels.
 *
 * <p><strong> u = [v_x, v_y, omega, v_fl, v_fr, v_rl, v_rr]ᵀ </strong> containing x velocity, y
 * velocity, and angular rate in the field coordinate system, followed by the velocity of the front
 * left, front right, rear left, and rear right wheels.
 *
 * <p><strong> y = [x, y, theta]ᵀ </strong> from vision containing x position, y position, and
 * heading; or <strong> y = [theta, s_fl, s_fr, s_rl, s_rr]ᵀ </strong> containing gyro heading,
 * followed by the distance driven by the front left, front right, rear left, and rear right wheels.
 */
public class MecanumDrivePoseEstimator {
  private final UnscentedKalmanFilter<N7, N7, N5> m_observer;
  private final MecanumDriveKinematics m_kinematics;
  private final BiConsumer<Matrix<N7, N1>, Matrix<N3, N1>> m_visionCorrect;
  private final TimeInterpolatableBuffer<Pose2d> m_poseBuffer;

  private final double m_nominalDt; // Seconds
  private double m_prevTimeSeconds = -1.0;

  private Rotation2d m_gyroOffset;
  private Rotation2d m_previousAngle;

  private Matrix<N3, N3> m_visionContR;

  /**
   * Constructs a MecanumDrivePoseEstimator.
   *
   * @param gyroAngle The current gyro angle.
   * @param initialPoseMeters The starting pose estimate.
   * @param wheelPositions The distances driven by each wheel.
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param stateStdDevs Standard deviations of model states. Increase these numbers to trust your
   *     model's state estimates less. This matrix is in the form [x, y, theta, s_fl, s_fr, s_rl,
   *     s_rr]ᵀ, with units in meters and radians, followed by meters.
   * @param localMeasurementStdDevs Standard deviation of the gyro measurement. Increase this number
   *     to trust sensor readings from the gyro less. This matrix is in the form [theta, s_fl, s_fr,
   *     s_rl, s_rr], with units in radians, followed by meters.
   * @param visionMeasurementStdDevs Standard deviations of the vision measurements. Increase these
   *     numbers to trust global measurements from vision less. This matrix is in the form [x, y,
   *     theta]ᵀ, with units in meters and radians.
   */
  public MecanumDrivePoseEstimator(
      Rotation2d gyroAngle,
      Pose2d initialPoseMeters,
      MecanumDriveWheelPositions wheelPositions,
      MecanumDriveKinematics kinematics,
      Matrix<N7, N1> stateStdDevs,
      Matrix<N5, N1> localMeasurementStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    this(
        gyroAngle,
        initialPoseMeters,
        wheelPositions,
        kinematics,
        stateStdDevs,
        localMeasurementStdDevs,
        visionMeasurementStdDevs,
        0.02);
  }

  /**
   * Constructs a MecanumDrivePoseEstimator.
   *
   * @param gyroAngle The current gyro angle.
   * @param initialPoseMeters The starting pose estimate.
   * @param wheelPositions The distances driven by each wheel.
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param stateStdDevs Standard deviations of model states. Increase these numbers to trust your
   *     model's state estimates less. This matrix is in the form [x, y, theta, s_fl, s_fr, s_rl,
   *     s_rr]ᵀ, with units in meters and radians, followed by meters.
   * @param localMeasurementStdDevs Standard deviation of the gyro measurement. Increase this number
   *     to trust sensor readings from the gyro less. This matrix is in the form [theta, s_fl, s_fr,
   *     s_rl, s_rr], with units in radians, followed by meters.
   * @param visionMeasurementStdDevs Standard deviations of the vision measurements. Increase these
   *     numbers to trust global measurements from vision less. This matrix is in the form [x, y,
   *     theta]ᵀ, with units in meters and radians.
   * @param nominalDtSeconds The time in seconds between each robot loop.
   */
  public MecanumDrivePoseEstimator(
      Rotation2d gyroAngle,
      Pose2d initialPoseMeters,
      MecanumDriveWheelPositions wheelPositions,
      MecanumDriveKinematics kinematics,
      Matrix<N7, N1> stateStdDevs,
      Matrix<N5, N1> localMeasurementStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs,
      double nominalDtSeconds) {
    m_nominalDt = nominalDtSeconds;

    m_observer =
        new UnscentedKalmanFilter<>(
            Nat.N7(),
            Nat.N5(),
            (x, u) -> u,
            (x, u) -> x.block(Nat.N5(), Nat.N1(), 2, 0),
            stateStdDevs,
            localMeasurementStdDevs,
            AngleStatistics.angleMean(2),
            AngleStatistics.angleMean(0),
            AngleStatistics.angleResidual(2),
            AngleStatistics.angleResidual(0),
            AngleStatistics.angleAdd(2),
            m_nominalDt);
    m_kinematics = kinematics;
    m_poseBuffer = TimeInterpolatableBuffer.createBuffer(1.5);

    // Initialize vision R
    setVisionMeasurementStdDevs(visionMeasurementStdDevs);

    m_visionCorrect =
        (u, y) ->
            m_observer.correct(
                Nat.N3(),
                u,
                y,
                (x, u1) -> x.block(Nat.N3(), Nat.N1(), 0, 0),
                m_visionContR,
                AngleStatistics.angleMean(2),
                AngleStatistics.angleResidual(2),
                AngleStatistics.angleResidual(2),
                AngleStatistics.angleAdd(2));

    m_gyroOffset = initialPoseMeters.getRotation().minus(gyroAngle);
    m_previousAngle = initialPoseMeters.getRotation();

    var poseVec = StateSpaceUtil.poseTo3dVector(initialPoseMeters);
    var xhat =
        VecBuilder.fill(
            poseVec.get(0, 0),
            poseVec.get(1, 0),
            poseVec.get(2, 0),
            wheelPositions.frontLeftMeters,
            wheelPositions.frontRightMeters,
            wheelPositions.rearLeftMeters,
            wheelPositions.rearRightMeters);

    m_observer.setXhat(xhat);
  }

  /**
   * Sets the pose estimator's trust of global measurements. This might be used to change trust in
   * vision measurements after the autonomous period, or to change trust as distance to a vision
   * target increases.
   *
   * @param visionMeasurementStdDevs Standard deviations of the vision measurements. Increase these
   *     numbers to trust global measurements from vision less. This matrix is in the form [x, y,
   *     theta]ᵀ, with units in meters and radians.
   */
  public void setVisionMeasurementStdDevs(Matrix<N3, N1> visionMeasurementStdDevs) {
    m_visionContR = StateSpaceUtil.makeCovarianceMatrix(Nat.N3(), visionMeasurementStdDevs);
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset in the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param poseMeters The position on the field that your robot is at.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The distances driven by each wheel.
   */
  public void resetPosition(
      Pose2d poseMeters, Rotation2d gyroAngle, MecanumDriveWheelPositions wheelPositions) {
    // Reset state estimate and error covariance
    m_observer.reset();
    m_poseBuffer.clear();

    var poseVec = StateSpaceUtil.poseTo3dVector(poseMeters);
    var xhat =
        VecBuilder.fill(
            poseVec.get(0, 0),
            poseVec.get(1, 0),
            poseVec.get(2, 0),
            wheelPositions.frontLeftMeters,
            wheelPositions.frontRightMeters,
            wheelPositions.rearLeftMeters,
            wheelPositions.rearRightMeters);

    m_observer.setXhat(xhat);

    m_prevTimeSeconds = -1;

    m_gyroOffset = getEstimatedPosition().getRotation().minus(gyroAngle);
    m_previousAngle = poseMeters.getRotation();
  }

  /**
   * Gets the pose of the robot at the current time as estimated by the Unscented Kalman Filter.
   *
   * @return The estimated robot pose in meters.
   */
  public Pose2d getEstimatedPosition() {
    return new Pose2d(
        m_observer.getXhat(0), m_observer.getXhat(1), new Rotation2d(m_observer.getXhat(2)));
  }

  /**
   * Add a vision measurement to the Unscented Kalman Filter. This will correct the odometry pose
   * estimate while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * MecanumDrivePoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link MecanumDrivePoseEstimator#updateWithTime}
   *     then you must use a timestamp with an epoch since FPGA startup (i.e. the epoch of this
   *     timestamp is the same epoch as Timer.getFPGATimestamp.) This means that you should use
   *     Timer.getFPGATimestamp as your time source or sync the epochs.
   */
  public void addVisionMeasurement(Pose2d visionRobotPoseMeters, double timestampSeconds) {
    var sample = m_poseBuffer.getSample(timestampSeconds);
    if (sample.isPresent()) {
      m_visionCorrect.accept(
          new MatBuilder<>(Nat.N7(), Nat.N1()).fill(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0),
          StateSpaceUtil.poseTo3dVector(
              getEstimatedPosition().transformBy(visionRobotPoseMeters.minus(sample.get()))));
    }
  }

  /**
   * Add a vision measurement to the Unscented Kalman Filter. This will correct the odometry pose
   * estimate while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * MecanumDrivePoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * <p>Note that the vision measurement standard deviations passed into this method will continue
   * to apply to future measurements until a subsequent call to {@link
   * MecanumDrivePoseEstimator#setVisionMeasurementStdDevs(Matrix)} or this method.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link MecanumDrivePoseEstimator#updateWithTime}
   *     then you must use a timestamp with an epoch since FPGA startup (i.e. the epoch of this
   *     timestamp is the same epoch as Timer.getFPGATimestamp.) This means that you should use
   *     Timer.getFPGATimestamp as your time source in this case.
   * @param visionMeasurementStdDevs Standard deviations of the vision measurements. Increase these
   *     numbers to trust global measurements from vision less. This matrix is in the form [x, y,
   *     theta]ᵀ, with units in meters and radians.
   */
  public void addVisionMeasurement(
      Pose2d visionRobotPoseMeters,
      double timestampSeconds,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    setVisionMeasurementStdDevs(visionMeasurementStdDevs);
    addVisionMeasurement(visionRobotPoseMeters, timestampSeconds);
  }

  /**
   * Updates the the Unscented Kalman Filter using only wheel encoder information. This should be
   * called every loop, and the correct loop period must be passed into the constructor of this
   * class.
   *
   * @param gyroAngle The current gyro angle.
   * @param wheelSpeeds The current speeds of the mecanum drive wheels.
   * @param wheelPositions The distances driven by each wheel.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d update(
      Rotation2d gyroAngle,
      MecanumDriveWheelSpeeds wheelSpeeds,
      MecanumDriveWheelPositions wheelPositions) {
    return updateWithTime(WPIUtilJNI.now() * 1.0e-6, gyroAngle, wheelSpeeds, wheelPositions);
  }

  /**
   * Updates the the Unscented Kalman Filter using only wheel encoder information. This should be
   * called every loop, and the correct loop period must be passed into the constructor of this
   * class.
   *
   * @param currentTimeSeconds Time at which this method was called, in seconds.
   * @param gyroAngle The current gyroscope angle.
   * @param wheelSpeeds The current speeds of the mecanum drive wheels.
   * @param wheelPositions The distances driven by each wheel.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d updateWithTime(
      double currentTimeSeconds,
      Rotation2d gyroAngle,
      MecanumDriveWheelSpeeds wheelSpeeds,
      MecanumDriveWheelPositions wheelPositions) {
    double dt = m_prevTimeSeconds >= 0 ? currentTimeSeconds - m_prevTimeSeconds : m_nominalDt;
    m_prevTimeSeconds = currentTimeSeconds;

    var angle = gyroAngle.plus(m_gyroOffset);
    var omega = angle.minus(m_previousAngle).getRadians() / dt;

    var chassisSpeeds = m_kinematics.toChassisSpeeds(wheelSpeeds);
    var fieldRelativeVelocities =
        new Translation2d(chassisSpeeds.vxMetersPerSecond, chassisSpeeds.vyMetersPerSecond)
            .rotateBy(angle);

    var u =
        VecBuilder.fill(
            fieldRelativeVelocities.getX(),
            fieldRelativeVelocities.getY(),
            omega,
            wheelSpeeds.frontLeftMetersPerSecond,
            wheelSpeeds.frontRightMetersPerSecond,
            wheelSpeeds.rearLeftMetersPerSecond,
            wheelSpeeds.rearRightMetersPerSecond);
    m_previousAngle = angle;

    var localY =
        VecBuilder.fill(
            angle.getRadians(),
            wheelPositions.frontLeftMeters,
            wheelPositions.frontRightMeters,
            wheelPositions.rearLeftMeters,
            wheelPositions.rearRightMeters);
    m_poseBuffer.addSample(currentTimeSeconds, getEstimatedPosition());
    m_observer.predict(u, dt);
    m_observer.correct(u, localY);

    return getEstimatedPosition();
  }
}
