package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.*;
import org.wpilib.math.util.MathSharedStore;

/**
 * Class for double follower wheel odometry. Odometry allows you to track the robot's position on
 * the field over a course of a match using course of a match using readings from 2 encoders and a
 * gyroscope.
 *
 * <p>This class is meant to be an easy replacement for {@link DoubleFollowerWheelOdometry}, only
 * requiring the addition of appropriate conversions between 2D and 3D versions of geometry
 * classes. (See {@link Pose3d#Pose3d(Pose2d)}, {@link Rotation3d#Rotation3d(Rotation2d)}, {@link
 * Translation3d#Translation3d(Translation2d)}, and {@link Pose3d#toPose2d()}.)
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 */
public class DoubleFollowerWheelOdometry3d extends Odometry3d<DoubleFollowerWheelPositions> {
  /**
   * Constructs a DoubleFollowerWheelOdometry3d object.
   * @param kinematics The double follower wheel kinematics.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The distances driven by each wheel and the angle reported by the gyroscope.
   * @param initialPose The starting position of the robot on the field.
   */
  public DoubleFollowerWheelOdometry3d(
      DoubleFollowerWheelKinematics kinematics,
      Rotation3d gyroAngle,
      DoubleFollowerWheelPositions wheelPositions,
      Pose3d initialPose) {
    super(kinematics, gyroAngle, wheelPositions, initialPose);

    MathSharedStore.reportUsage("DoubleFollowerWheelOdometry3d", "");
  }

  /**
   * Updates the robot's position on the field using forward kinematics and integration of the pose
   * over time.
   * @param x The distance traveled by the forward-facing wheel in meters.
   * @param y The distance traveled by the left-facing wheel in meters.
   * @param gyroAngle The angle reported by the gyroscope.
   * @return The new pose of the robot.
   */
  public Pose3d update(double x, double y, Rotation3d gyroAngle) {
    return update(gyroAngle,new DoubleFollowerWheelPositions(x, y, gyroAngle.toRotation2d()));
  }
}
