package org.wpilib.math.estimator;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.geometry.Translation3d;
import org.wpilib.math.kinematics.DoubleFollowerWheelKinematics;
import org.wpilib.math.kinematics.DoubleFollowerWheelOdometry3d;
import org.wpilib.math.kinematics.DoubleFollowerWheelPositions;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N4;

/**
 * This class wraps {@link DoubleFollowerWheelOdometry3d Double Follower Wheel Odometry} to fuse
 * latency-compensated vision measurements with double follower wheel encoder measurements. It will
 * correct for noisy measurements and encoder drift. It is intended to be a drop-in replacement for
 * {@link DoubleFollowerWheelOdometry3d}. It is also intended to be an easy replacement for {@link
 * DoubleFollowerWheelPoseEstimator}, only requiring the addition of a standard deviation for Z and
 * appropriate conversions between 2D and 3D versions of geometry classes. (See {@link
 * Pose3d#Pose3d(Pose2d)}, {@link Rotation3d#Rotation3d(Rotation2d)}, {@link
 * Translation3d#Translation3d(Translation2d)}, and {@link Pose3d#toPose2d()}.)
 *
 * <p>{@link DoubleFollowerWheelPoseEstimator3d#update} should be called every robot loop.
 *
 * <p>{@link DoubleFollowerWheelPoseEstimator3d#addVisionMeasurement} can be called as infrequently
 * as you want; if you never call it, then this class will behave mostly like regular encoder
 * odometry.
 */
public class DoubleFollowerWheelPoseEstimator3d
    extends PoseEstimator3d<DoubleFollowerWheelPositions> {
  /**
   * Constructs a DoubleFollowerWheelPoseEstimator3d with default standard deviations for the model
   * and vision measurements.
   *
   * <p>The default standard deviations of the model states are 0.1 meters for x, 0.1 meters for y,
   * 0.1 meters for z, and 0.1 radians for angle. The default standard deviations of the vision
   * measurements are 0.9 meters for x, 0.9 meters for y, 0.9 meters for z, and 0.9 radians for
   * angle.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The distances and angles measured by the follower wheels.
   * @param initialPose The starting pose estimate.
   */
  public DoubleFollowerWheelPoseEstimator3d(
      DoubleFollowerWheelKinematics kinematics,
      Rotation3d gyroAngle,
      DoubleFollowerWheelPositions wheelPositions,
      Pose3d initialPose) {
    super(
        kinematics,
        new DoubleFollowerWheelOdometry3d(kinematics, gyroAngle, wheelPositions, initialPose),
        VecBuilder.fill(0.1, 0.1, 0.1, 0.1),
        VecBuilder.fill(0.9, 0.9, 0.9, 0.9));
  }

  /**
   * Constructs a DoubleFollowerWheelPoseEstimator3d.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The gyro angle of the robot.
   * @param wheelPositions The distances and angles measured by the follower wheels.
   * @param initialPose The estimated initial pose.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in meters, y position
   *     in meters, z position in meters, and heading in radians). Increase these numbers to trust
   *     your state estimate less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, z position in meters, and heading in radians). Increase
   *     these numbers to trust the vision pose measurement less.
   */
  public DoubleFollowerWheelPoseEstimator3d(
      DoubleFollowerWheelKinematics kinematics,
      Rotation3d gyroAngle,
      DoubleFollowerWheelPositions wheelPositions,
      Pose3d initialPose,
      Matrix<N4, N1> stateStdDevs,
      Matrix<N4, N1> visionMeasurementStdDevs) {
    super(
        kinematics,
        new DoubleFollowerWheelOdometry3d(kinematics, gyroAngle, wheelPositions, initialPose),
        stateStdDevs,
        visionMeasurementStdDevs);
  }
}
