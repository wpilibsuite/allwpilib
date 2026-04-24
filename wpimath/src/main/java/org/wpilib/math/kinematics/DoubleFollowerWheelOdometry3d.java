package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;

public class DoubleFollowerWheelOdometry3d extends Odometry3d<DoubleFollowerWheelPositions> {
  public DoubleFollowerWheelOdometry3d(
      DoubleFollowerWheelKinematics kinematics,
      Rotation3d gyroAngle,
      DoubleFollowerWheelPositions wheelPositions,
      Pose3d initialPose) {
    super(kinematics, gyroAngle, wheelPositions, initialPose);
  }

  public Pose3d update(double x, double y, Rotation3d gyroAngle) {
    return update(gyroAngle,new DoubleFollowerWheelPositions(x, y, gyroAngle.toRotation2d()));
  }
}
