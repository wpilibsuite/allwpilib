package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;

public class TripleFollowerWheelOdometry extends Odometry<TripleFollowerWheelPositions> {
  public TripleFollowerWheelOdometry(
      TripleFollowerWheelKinematics kinematics,
      Rotation2d gyroAngle,
      TripleFollowerWheelPositions wheelPositions,
      Pose2d initialPose) {
    super(kinematics, gyroAngle, wheelPositions, initialPose);
  }
}
