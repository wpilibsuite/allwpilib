package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;

public class TripleFollowerWheelOdometry3d extends Odometry3d<TripleFollowerWheelPositions> {
  public TripleFollowerWheelOdometry3d(
      TripleFollowerWheelKinematics kinematics,
      Rotation3d gyroAngle,
      TripleFollowerWheelPositions wheelPositions,
      Pose3d initialPose) {
    super(kinematics, gyroAngle, wheelPositions, initialPose);
  }
}
