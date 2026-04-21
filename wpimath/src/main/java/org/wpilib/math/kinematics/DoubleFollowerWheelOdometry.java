package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Pose2d;

public class DoubleFollowerWheelOdometry extends Odometry<DoubleFollowerWheelPositions> {
  public DoubleFollowerWheelOdometry(
      DoubleFollowerWheelKinematics kinematics,
      DoubleFollowerWheelPositions wheelPositions,
      Pose2d initialPose) {
    super(kinematics, wheelPositions.gyroTheta, wheelPositions, initialPose);
  }

  public void resetPosition(DoubleFollowerWheelPositions wheelPositions, Pose2d pose) {
    super.resetPosition(wheelPositions.gyroTheta, wheelPositions, pose);
  }

  public Pose2d update(DoubleFollowerWheelPositions wheelPositions) {
    return super.update(wheelPositions.gyroTheta, wheelPositions);
  }
}
