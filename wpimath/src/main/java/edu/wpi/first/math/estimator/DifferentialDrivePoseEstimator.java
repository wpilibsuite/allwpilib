// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.interpolation.TimeInterpolatableBuffer;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.kinematics.DifferentialDriveOdometry;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.util.WPIUtilJNI;
import java.util.Objects;

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
  private final DifferentialDriveKinematics m_kinematics;
  private final DifferentialDriveOdometry m_odometry;
  private final Matrix<N3, N1> m_q = new Matrix<>(Nat.N3(), Nat.N1());
  private Matrix<N3, N3> m_visionK = new Matrix<>(Nat.N3(), Nat.N3());
  private Matrix<N3, N3> m_visionKSum = Matrix.eye(Nat.N3());
  private Pose2d m_poseEstimate;

  private final TimeInterpolatableBuffer<InterpolationRecord> m_poseBuffer =
      TimeInterpolatableBuffer.createBuffer(1.5);

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
    m_kinematics = kinematics;
    m_odometry =
        new DifferentialDriveOdometry(
            gyroAngle, leftDistanceMeters, rightDistanceMeters, initialPoseMeters);
    m_poseEstimate = m_odometry.getPoseMeters();

    for (int i = 0; i < 3; ++i) {
      m_q.set(i, 0, stateStdDevs.get(i, 0) * stateStdDevs.get(i, 0));
    }

    // Initialize vision R
    setVisionMeasurementStdDevs(visionMeasurementStdDevs);
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
    var r = new double[3];
    for (int i = 0; i < 3; ++i) {
      r[i] = visionMeasurementStdDevs.get(i, 0) * visionMeasurementStdDevs.get(i, 0);
    }

    // Solve for closed form Kalman gain for continuous Kalman filter with A = 0
    // and C = I. See wpimath/algorithms.md.
    for (int row = 0; row < 3; ++row) {
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
      Rotation2d gyroAngle,
      double leftPositionMeters,
      double rightPositionMeters,
      Pose2d poseMeters) {
    // Reset state estimate and error covariance
    m_odometry.resetPosition(gyroAngle, leftPositionMeters, rightPositionMeters, poseMeters);
    m_poseEstimate = poseMeters;
    m_poseBuffer.clear();
    m_visionKSum = Matrix.eye(Nat.N3());
  }

  /**
   * Gets the estimated robot pose.
   *
   * @return The estimated robot pose in meters.
   */
  public Pose2d getEstimatedPosition() {
    return m_poseEstimate;
  }

  /**
   * Gets the estimated robot pose at timestampSeconds.
   *
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link
   *     DifferentialDrivePoseEstimator#updateWithTime(double,Rotation2d,double,double)} then you
   *     must use a timestamp with an epoch since FPGA startup (i.e., the epoch of this timestamp is
   *     the same epoch as {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()}.) This means that
   *     you should use {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()} as your time source
   *     or sync the epochs.
   * @return The estimated robot pose in meters.
   */
  public Pose2d getEstimatedPosition(double timestampSeconds) {
    if (m_poseBuffer.getInternalBuffer().isEmpty()) {
      return m_poseEstimate;
    }
    // current to old odometry delta
    var delta =
        new Transform2d(
            m_odometry.getPoseMeters(), m_poseBuffer.getSample(timestampSeconds).get().poseMeters);
    return m_poseEstimate.transformBy(delta);
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
    // Step 1: Get the odometry pose measured at the moment the vision measurement was made.
    var sample = m_poseBuffer.getSample(timestampSeconds);

    if (sample.isEmpty()) {
      return;
    }

    // Step 2: Get the odometry delta in the time since the vision measurement,
    var delta = new Transform2d(sample.get().poseMeters, m_odometry.getPoseMeters());
    // and apply its inverse to the current pose estimate to get the estimate at the time
    // of the vision measurement
    var old_estimate = m_poseEstimate.transformBy(delta.inverse());

    // Step 3: Measure the twist between the "old" estimated pose and the vision pose
    var twist = old_estimate.log(visionRobotPoseMeters);

    // Step 4: We should not trust the twist entirely, so instead we scale this twist by a Kalman
    // gain matrix representing how much we trust vision measurements compared to our current pose.
    var k_times_twist = m_visionK.times(VecBuilder.fill(twist.dx, twist.dy, twist.dtheta));

    // Step 5: The Kalman gains scale the vision twists independent of other vision measurements,
    // so applying multiple vision measurements before the next update will have different results
    // depending on the order they are applied in. We can sum the Kalman gains applied before the
    // next update and use that to effectively average the twist for multiple vision measurements.
    var weighted_k_times_twist =
        k_times_twist.elementTimes(m_visionKSum.diag().extractColumnVector(0).elementPower(-1));
    m_visionKSum = m_visionKSum.plus(m_visionK);

    // Step 5: Convert back to Twist2d.
    double[] scaledTwistVals = weighted_k_times_twist.getData();
    var scaledTwist = new Twist2d(scaledTwistVals[0], scaledTwistVals[1], scaledTwistVals[2]);

    // Step 6: Apply scaled twist to the "old" estimated pose
    old_estimate = old_estimate.exp(scaledTwist);

    // Step 7: Re-apply odometry delta to get the "current" estimated pose
    m_poseEstimate = old_estimate.transformBy(delta);
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
    addVisionMeasurement(visionRobotPoseMeters, timestampSeconds);
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
    return updateWithTime(
        WPIUtilJNI.now() * 1.0e-6, gyroAngle, distanceLeftMeters, distanceRightMeters);
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
    // update odometry
    var lastOdom = m_odometry.getPoseMeters();
    var currOdom = m_odometry.update(gyroAngle, distanceLeftMeters, distanceRightMeters);
    m_poseBuffer.addSample(
        currentTimeSeconds,
        new InterpolationRecord(currOdom, distanceLeftMeters, distanceRightMeters));

    // apply odometry update to pose estimate
    m_poseEstimate = m_poseEstimate.transformBy(new Transform2d(lastOdom, currOdom));

    // reset kalman gain sum matrix
    m_visionKSum = Matrix.eye(Nat.N3());

    return getEstimatedPosition();
  }

  /**
   * Represents an odometry record. The record contains the inputs provided as well as the pose that
   * was observed based on these inputs, as well as the previous record and its inputs.
   */
  private class InterpolationRecord implements Interpolatable<InterpolationRecord> {
    // The pose observed given the current sensor inputs and the previous pose.
    private final Pose2d poseMeters;

    // The distance traveled by the left encoder.
    private final double leftMeters;

    // The distance traveled by the right encoder.
    private final double rightMeters;

    /**
     * Constructs an Interpolation Record with the specified parameters.
     *
     * @param pose The pose observed given the current sensor inputs and the previous pose.
     * @param leftMeters The distance traveled by the left encoder.
     * @param rightMeters The distanced traveled by the right encoder.
     */
    private InterpolationRecord(Pose2d poseMeters, double leftMeters, double rightMeters) {
      this.poseMeters = poseMeters;
      this.leftMeters = leftMeters;
      this.rightMeters = rightMeters;
    }

    /**
     * Return the interpolated record. This object is assumed to be the starting position, or lower
     * bound.
     *
     * @param endValue The upper bound, or end.
     * @param t How far between the lower and upper bound we are. This should be bounded in [0, 1].
     * @return The interpolated value.
     */
    @Override
    public InterpolationRecord interpolate(InterpolationRecord endValue, double t) {
      if (t < 0) {
        return this;
      } else if (t >= 1) {
        return endValue;
      } else {
        // Find the new left distance.
        var left_lerp = MathUtil.interpolate(this.leftMeters, endValue.leftMeters, t);

        // Find the new right distance.
        var right_lerp = MathUtil.interpolate(this.rightMeters, endValue.rightMeters, t);

        // Find the new gyro angle.
        var gyro_lerp = poseMeters.getRotation().interpolate(endValue.poseMeters.getRotation(), t);

        // Create a twist to represent this change based on the interpolated sensor inputs.
        Twist2d twist = m_kinematics.toTwist2d(left_lerp - leftMeters, right_lerp - rightMeters);
        twist.dtheta = gyro_lerp.minus(poseMeters.getRotation()).getRadians();

        return new InterpolationRecord(poseMeters.exp(twist), left_lerp, right_lerp);
      }
    }

    @Override
    public boolean equals(Object obj) {
      if (this == obj) {
        return true;
      }
      if (!(obj instanceof InterpolationRecord)) {
        return false;
      }
      InterpolationRecord record = (InterpolationRecord) obj;
      return Double.compare(leftMeters, record.leftMeters) == 0
          && Double.compare(rightMeters, record.rightMeters) == 0
          && Objects.equals(poseMeters, record.poseMeters);
    }

    @Override
    public int hashCode() {
      return Objects.hash(leftMeters, rightMeters, poseMeters);
    }
  }
}
