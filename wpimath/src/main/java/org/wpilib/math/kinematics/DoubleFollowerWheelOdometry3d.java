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
}
