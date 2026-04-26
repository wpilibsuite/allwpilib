package org.wpilib.math.estimator;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.kinematics.*;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N3;

/**
 * This class wraps {@link DoubleFollowerWheelOdometry Double Follower Wheel Odometry} to fuse
 * latency-compensated vision measurements with double follower wheel encoder measurements. It will
 * correct for noisy measurements and encoder drift. It is intended to be a drop-in replacement for
 * {@link DoubleFollowerWheelOdometry}.
 *
 * <p>{@link DoubleFollowerWheelPoseEstimator#update} should be called every robot loop.
 *
 * <p>{@link DoubleFollowerWheelPoseEstimator#addVisionMeasurement} can be called as infrequently
 * as you want; if you never call it, then this class will behave mostly like regular encoder
 * odometry.
 */
public class DoubleFollowerWheelPoseEstimator extends PoseEstimator<DoubleFollowerWheelPositions> {
  /**
   * Constructs a DoubleFollowerWheelPoseEstimator with default standard deviations for the model
   * and vision measurements.
   *
   * <p>The default standard deviations of the model states are 0.1 meters for x, 0.1 meters for
   * y, and 0.1 radians for heading. The default standard deviations of the vision measurements are
   * 0.9 meters for x, 0.9 meters for y, and 0.9 radians for heading.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param wheelPositions The distances and angles measured by the follower wheels.
   * @param initialPose The starting pose estimate.
   */
  public DoubleFollowerWheelPoseEstimator(
      DoubleFollowerWheelKinematics kinematics,
      DoubleFollowerWheelPositions wheelPositions,
      Pose2d initialPose) {
    this(
        kinematics,
        wheelPositions,
        initialPose,
        VecBuilder.fill(0.1, 0.1, 0.1),
        VecBuilder.fill(0.9, 0.9, 0.9));
  }

  /**
   * Constructs a DoubleFollowerWheelPoseEstimator.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param wheelPositions The distances and angles measured by the follower wheels.
   * @param initialPose The starting pose estimate.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in meters, y position
   *     in meters, and heading in radians). Increase these numbers to trust your state estimate
   *     less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public DoubleFollowerWheelPoseEstimator(
      DoubleFollowerWheelKinematics kinematics,
      DoubleFollowerWheelPositions wheelPositions,
      Pose2d initialPose,
      Matrix<N3, N1> stateStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    super(
        kinematics,
        new DoubleFollowerWheelOdometry(kinematics, wheelPositions, initialPose),
        stateStdDevs,
        visionMeasurementStdDevs);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called
   * every loop.
   *
   * @param wheelPositions The distances and angles measured by the follower wheels.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d update(DoubleFollowerWheelPositions wheelPositions) {
    return super.update(wheelPositions.theta, wheelPositions);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called
   * every loop.
   *
   * @param currentTime Time at which this method was called, in seconds.
   * @param wheelPositions The distances and angles measured by the follower wheels.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d updateWithTime(double currentTime, DoubleFollowerWheelPositions wheelPositions) {
    return super.updateWithTime(currentTime, wheelPositions.theta, wheelPositions);
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param wheelPositions The distances and angles measured by the follower wheels.
   * @param pose The position on the field that your robot is at.
   */
  public void resetPosition(DoubleFollowerWheelPositions wheelPositions, Pose2d pose) {
    super.resetPosition(wheelPositions.theta, wheelPositions, pose);
  }
}
