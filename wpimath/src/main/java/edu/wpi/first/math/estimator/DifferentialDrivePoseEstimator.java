// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Twist3d;
import edu.wpi.first.math.interpolation.TimeInterpolatableBuffer;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.kinematics.DifferentialDriveOdometry;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.numbers.N4;
import java.util.NoSuchElementException;

/**
 * This class wraps {@link DifferentialDriveOdometry Differential Drive Odometry} to fuse
 * latency-compensated vision measurements with differential drive encoder measurements. It is
 * intended to be a drop-in replacement for {@link DifferentialDriveOdometry}; in fact, if you never
 * call {@link DifferentialDrivePoseEstimator#addVisionMeasurement} and only call {@link
 * DifferentialDrivePoseEstimator#update} then this will behave exactly the same as
 * DifferentialDriveOdometry.
 *
 * <p>{@link DifferentialDrivePoseEstimator#update} should be called every robot loop.
 *
 * <p>{@link DifferentialDrivePoseEstimator#addVisionMeasurement} can be called as infrequently as
 * you want; if you never call it then this class will behave exactly like regular encoder odometry.
 */
public class DifferentialDrivePoseEstimator {
  private final DifferentialDriveOdometry m_odometry;
  private final Matrix<N4, N1> m_q = new Matrix<>(Nat.N4(), Nat.N1());
  private Matrix<N4, N4> m_visionK = new Matrix<>(Nat.N4(), Nat.N4());

  private static final double kBufferDuration = 1.5;

  private final TimeInterpolatableBuffer<Pose3d> m_poseBuffer =
      TimeInterpolatableBuffer.createBuffer(kBufferDuration);

  private Pose3d m_poseEstimate;

  /**
   * Constructs a DifferentialDrivePoseEstimator with default standard deviations for the model and
   * vision measurements.
   *
   * <p>The default standard deviations of the model states are 0.02 meters for x, y, and z, and
   * 0.01 radians for heading. The default standard deviations of the vision measurements are 0.1
   * meters for x, y, and z, and 0.1 radians for heading.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param leftDistanceMeters The distance traveled by the left encoder.
   * @param rightDistanceMeters The distance traveled by the right encoder.
   * @param initialPoseMeters The starting pose estimate.
   */
  public DifferentialDrivePoseEstimator(
      DifferentialDriveKinematics kinematics,
      Rotation3d gyroAngle,
      double leftDistanceMeters,
      double rightDistanceMeters,
      Pose3d initialPoseMeters) {
    this(
        kinematics,
        gyroAngle,
        leftDistanceMeters,
        rightDistanceMeters,
        initialPoseMeters,
        VecBuilder.fill(0.02, 0.02, 0.02, 0.01),
        VecBuilder.fill(0.1, 0.1, 0.1, 0.1));
  }

  /**
   * Constructs a DifferentialDrivePoseEstimator with default standard deviations for the model and
   * vision measurements.
   *
   * <p>The default standard deviations of the model states are 0.02 meters for x, 0.02 meters for
   * y, and 0.01 radians for heading. The default standard deviations of the vision measurements are
   * 0.1 meters for x, 0.1 meters for y, and 0.1 radians for heading.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param leftDistanceMeters The distance traveled by the left encoder.
   * @param rightDistanceMeters The distance traveled by the right encoder.
   * @param initialPoseMeters The starting pose estimate.
   */
  public DifferentialDrivePoseEstimator(
      DifferentialDriveKinematics kinematics,
      Rotation2d gyroAngle,
      double leftDistanceMeters,
      double rightDistanceMeters,
      Pose2d initialPoseMeters) {
    this(
        kinematics,
        gyroAngle,
        leftDistanceMeters,
        rightDistanceMeters,
        initialPoseMeters,
        VecBuilder.fill(0.02, 0.02, 0.01),
        VecBuilder.fill(0.1, 0.1, 0.1));
  }

  /**
   * Constructs a DifferentialDrivePoseEstimator.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The gyro angle of the robot.
   * @param leftDistanceMeters The distance traveled by the left encoder.
   * @param rightDistanceMeters The distance traveled by the right encoder.
   * @param initialPoseMeters The estimated initial pose.
   * @param stateStdDevs Standard deviations of the pose estimate (x, y, and z position in meters,
   *     with heading in radians). Increase these numbers to trust your state estimate less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x, y, and z
   *     position in meters, with heading in radians). Increase these numbers to trust the vision
   *     pose measurement less.
   */
  public DifferentialDrivePoseEstimator(
      DifferentialDriveKinematics kinematics,
      Rotation3d gyroAngle,
      double leftDistanceMeters,
      double rightDistanceMeters,
      Pose3d initialPoseMeters,
      Matrix<N4, N1> stateStdDevs,
      Matrix<N4, N1> visionMeasurementStdDevs) {
    m_odometry =
        new DifferentialDriveOdometry(
            gyroAngle, leftDistanceMeters, rightDistanceMeters, initialPoseMeters);
    m_poseEstimate = m_odometry.getPoseMeters3d();

    for (int i = 0; i < 4; ++i) {
      m_q.set(i, 0, stateStdDevs.get(i, 0) * stateStdDevs.get(i, 0));
    }

    // Initialize vision R
    setVisionMeasurementStdDevs3d(visionMeasurementStdDevs);
  }

  /**
   * Constructs a DifferentialDrivePoseEstimator.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The gyro angle of the robot.
   * @param leftDistanceMeters The distance traveled by the left encoder.
   * @param rightDistanceMeters The distance traveled by the right encoder.
   * @param initialPoseMeters The estimated initial pose.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in meters, y position
   *     in meters, and heading in radians). Increase these numbers to trust your state estimate
   *     less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public DifferentialDrivePoseEstimator(
      DifferentialDriveKinematics kinematics,
      Rotation2d gyroAngle,
      double leftDistanceMeters,
      double rightDistanceMeters,
      Pose2d initialPoseMeters,
      Matrix<N3, N1> stateStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    this(
        kinematics,
        new Rotation3d(0, 0, gyroAngle.getRadians()),
        leftDistanceMeters,
        rightDistanceMeters,
        new Pose3d(initialPoseMeters),
        VecBuilder.fill(stateStdDevs.get(0, 0), stateStdDevs.get(1, 0), 0, stateStdDevs.get(2, 0)),
        VecBuilder.fill(
            visionMeasurementStdDevs.get(0, 0),
            visionMeasurementStdDevs.get(1, 0),
            0,
            visionMeasurementStdDevs.get(2, 0)));
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
    setVisionMeasurementStdDevs3d(
        VecBuilder.fill(
            visionMeasurementStdDevs.get(0, 0),
            visionMeasurementStdDevs.get(1, 0),
            0,
            visionMeasurementStdDevs.get(2, 0)));
  }

  /**
   * Sets the pose estimator's trust of global measurements. This might be used to change trust in
   * vision measurements after the autonomous period, or to change trust as distance to a vision
   * target increases.
   *
   * @param visionMeasurementStdDevs Standard deviations of the vision measurements. Increase these
   *     numbers to trust global measurements from vision less. This matrix is in the form [x, y, z,
   *     heading]ᵀ, with units in meters and radians.
   */
  public void setVisionMeasurementStdDevs3d(Matrix<N4, N1> visionMeasurementStdDevs) {
    var r = new double[4];
    for (int i = 0; i < 4; ++i) {
      r[i] = visionMeasurementStdDevs.get(i, 0) * visionMeasurementStdDevs.get(i, 0);
    }

    // Solve for closed form Kalman gain for continuous Kalman filter with A = 0
    // and C = I. See wpimath/algorithms.md.
    for (int row = 0; row < 4; ++row) {
      if (m_q.get(row, 0) == 0.0) {
        m_visionK.set(row, row, 0.0);
      } else {
        m_visionK.set(
            row, row, m_q.get(row, 0) / (m_q.get(row, 0) + Math.sqrt(m_q.get(row, 0) * r[row])));
      }
    }
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftPositionMeters The distance traveled by the left encoder.
   * @param rightPositionMeters The distance traveled by the right encoder.
   * @param poseMeters The position on the field that your robot is at.
   */
  public void resetPosition(
      Rotation3d gyroAngle,
      double leftPositionMeters,
      double rightPositionMeters,
      Pose3d poseMeters) {
    // Reset state estimate and error covariance
    m_odometry.resetPosition(gyroAngle, leftPositionMeters, rightPositionMeters, poseMeters);
    m_poseBuffer.clear();
    m_poseEstimate = m_odometry.getPoseMeters3d();
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftPositionMeters The distance traveled by the left encoder.
   * @param rightPositionMeters The distance traveled by the right encoder.
   * @param poseMeters The position on the field that your robot is at.
   */
  public void resetPosition(
      Rotation2d gyroAngle,
      double leftPositionMeters,
      double rightPositionMeters,
      Pose2d poseMeters) {
    resetPosition(
        new Rotation3d(0, 0, gyroAngle.getRadians()),
        leftPositionMeters,
        rightPositionMeters,
        new Pose3d(poseMeters));
  }

  /**
   * Gets the estimated robot pose.
   *
   * @return The estimated robot pose in meters.
   */
  public Pose2d getEstimatedPosition() {
    return m_poseEstimate.toPose2d();
  }

  /**
   * Gets the estimated robot pose.
   *
   * @return The estimated robot pose in meters.
   */
  public Pose3d getEstimatedPosition3d() {
    return m_poseEstimate;
  }

  /**
   * Adds a vision measurement to the Kalman Filter. This will correct the odometry pose estimate
   * while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * DifferentialDrivePoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link
   *     DifferentialDrivePoseEstimator#updateWithTime(double,Rotation2d,double,double)} then you
   *     must use a timestamp with an epoch since FPGA startup (i.e., the epoch of this timestamp is
   *     the same epoch as {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()}.) This means that
   *     you should use {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()} as your time source
   *     or sync the epochs.
   */
  public void addVisionMeasurement(Pose2d visionRobotPoseMeters, double timestampSeconds) {
    addVisionMeasurement(new Pose3d(visionRobotPoseMeters), timestampSeconds);
  }

  /**
   * Adds a vision measurement to the Kalman Filter. This will correct the odometry pose estimate
   * while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * DifferentialDrivePoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link
   *     DifferentialDrivePoseEstimator#updateWithTime(double,Rotation2d,double,double)} then you
   *     must use a timestamp with an epoch since FPGA startup (i.e., the epoch of this timestamp is
   *     the same epoch as {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()}.) This means that
   *     you should use {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()} as your time source
   *     or sync the epochs.
   */
  public void addVisionMeasurement(Pose3d visionRobotPoseMeters, double timestampSeconds) {
    // Step 0: If this measurement is old enough to be outside the pose buffer's timespan, skip.
    try {
      if (m_poseBuffer.getInternalBuffer().lastKey() - kBufferDuration > timestampSeconds) {
        return;
      }
    } catch (NoSuchElementException ex) {
      return;
    }

    // Step 1: Get the pose odometry measured at the moment the vision measurement was made.
    var sample = m_poseBuffer.getSample(timestampSeconds);

    if (sample.isEmpty()) {
      return;
    }

    var record = sample.get();

    var odometry_backtrack = m_odometry.getPoseMeters3d().log(record);
    var odometry_fastforward =
        new Twist3d(
            -odometry_backtrack.dx,
            -odometry_backtrack.dy,
            -odometry_backtrack.dz,
            -odometry_backtrack.rx,
            -odometry_backtrack.ry,
            -odometry_backtrack.rz);

    var old_estimate = m_poseEstimate.exp(odometry_backtrack);

    // Step 2: Measure the twist between the odometry pose and the vision pose.
    var twist = old_estimate.log(visionRobotPoseMeters);

    // Step 3: We should not trust the twist entirely, so instead we scale this twist by a Kalman
    // gain matrix representing how much we trust vision measurements compared to our current pose.
    var twist_rvec = VecBuilder.fill(twist.rx, twist.ry, twist.rz);
    var twist_angle = twist_rvec.norm();
    var k_times_twist = m_visionK.times(VecBuilder.fill(twist.dx, twist.dy, twist.dz, twist_angle));

    // Step 4: Convert back to Twist3d.
    var scaledTwist =
        new Twist3d(
            k_times_twist.get(0, 0),
            k_times_twist.get(1, 0),
            k_times_twist.get(2, 0),
            twist_rvec.get(0, 0) / twist_angle * k_times_twist.get(3, 0),
            twist_rvec.get(1, 0) / twist_angle * k_times_twist.get(3, 0),
            twist_rvec.get(2, 0) / twist_angle * k_times_twist.get(3, 0));

    old_estimate = old_estimate.exp(scaledTwist);

    m_poseEstimate = old_estimate.exp(odometry_fastforward);
  }

  /**
   * Adds a vision measurement to the Kalman Filter. This will correct the odometry pose estimate
   * while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * DifferentialDrivePoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * <p>Note that the vision measurement standard deviations passed into this method will continue
   * to apply to future measurements until a subsequent call to {@link
   * DifferentialDrivePoseEstimator#setVisionMeasurementStdDevs(Matrix)} or this method.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link
   *     DifferentialDrivePoseEstimator#updateWithTime(double,Rotation2d,double,double)}, then you
   *     must use a timestamp with an epoch since FPGA startup (i.e., the epoch of this timestamp is
   *     the same epoch as {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()}). This means that
   *     you should use {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()} as your time source
   *     in this case.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public void addVisionMeasurement(
      Pose3d visionRobotPoseMeters,
      double timestampSeconds,
      Matrix<N4, N1> visionMeasurementStdDevs) {
    setVisionMeasurementStdDevs3d(visionMeasurementStdDevs);
    addVisionMeasurement(visionRobotPoseMeters, timestampSeconds);
  }

  /**
   * Adds a vision measurement to the Kalman Filter. This will correct the odometry pose estimate
   * while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * DifferentialDrivePoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * <p>Note that the vision measurement standard deviations passed into this method will continue
   * to apply to future measurements until a subsequent call to {@link
   * DifferentialDrivePoseEstimator#setVisionMeasurementStdDevs(Matrix)} or this method.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link
   *     DifferentialDrivePoseEstimator#updateWithTime(double,Rotation2d,double,double)}, then you
   *     must use a timestamp with an epoch since FPGA startup (i.e., the epoch of this timestamp is
   *     the same epoch as {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()}). This means that
   *     you should use {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()} as your time source
   *     in this case.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public void addVisionMeasurement(
      Pose2d visionRobotPoseMeters,
      double timestampSeconds,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    setVisionMeasurementStdDevs(visionMeasurementStdDevs);
    addVisionMeasurement(new Pose3d(visionRobotPoseMeters), timestampSeconds);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param gyroAngle The current gyro angle.
   * @param distanceLeftMeters The total distance travelled by the left wheel in meters.
   * @param distanceRightMeters The total distance travelled by the right wheel in meters.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d update(
      Rotation2d gyroAngle, double distanceLeftMeters, double distanceRightMeters) {
    return update(
            new Rotation3d(0, 0, gyroAngle.getRadians()), distanceLeftMeters, distanceRightMeters)
        .toPose2d();
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param gyroAngle The current gyro angle.
   * @param distanceLeftMeters The total distance travelled by the left wheel in meters.
   * @param distanceRightMeters The total distance travelled by the right wheel in meters.
   * @return The estimated pose of the robot in meters.
   */
  public Pose3d update(
      Rotation3d gyroAngle, double distanceLeftMeters, double distanceRightMeters) {
    return updateWithTime(
        MathSharedStore.getTimestamp(), gyroAngle, distanceLeftMeters, distanceRightMeters);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param currentTimeSeconds Time at which this method was called, in seconds.
   * @param gyroAngle The current gyro angle.
   * @param distanceLeftMeters The total distance travelled by the left wheel in meters.
   * @param distanceRightMeters The total distance travelled by the right wheel in meters.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d updateWithTime(
      double currentTimeSeconds,
      Rotation2d gyroAngle,
      double distanceLeftMeters,
      double distanceRightMeters) {
    return updateWithTime(
            currentTimeSeconds,
            new Rotation3d(0, 0, gyroAngle.getRadians()),
            distanceLeftMeters,
            distanceRightMeters)
        .toPose2d();
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param currentTimeSeconds Time at which this method was called, in seconds.
   * @param gyroAngle The current gyro angle.
   * @param distanceLeftMeters The total distance travelled by the left wheel in meters.
   * @param distanceRightMeters The total distance travelled by the right wheel in meters.
   * @return The estimated pose of the robot in meters.
   */
  public Pose3d updateWithTime(
      double currentTimeSeconds,
      Rotation3d gyroAngle,
      double distanceLeftMeters,
      double distanceRightMeters) {
    var lastOdom = m_odometry.getPoseMeters3d();
    var currOdom = m_odometry.update(gyroAngle, distanceLeftMeters, distanceRightMeters);
    m_poseBuffer.addSample(currentTimeSeconds, currOdom);

    m_poseEstimate = m_poseEstimate.exp(lastOdom.log(currOdom));

    return getEstimatedPosition3d();
  }
}
