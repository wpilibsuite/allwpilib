package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;

public class DoubleFollowerWheelOdometry extends Odometry<DoubleFollowerWheelPositions> {
  public DoubleFollowerWheelOdometry(
      DoubleFollowerWheelKinematics kinematics,
      DoubleFollowerWheelPositions wheelPositions,
      Pose2d initialPose) {
    super(kinematics, wheelPositions.theta, wheelPositions, initialPose);
  }

  // TODO: Should I do an impl that doesn't extend Odometry<> since the super methods that take
  //  in redundant gyro angles could be confusing?
  //  But also that would be duplicated, redundant code
  public void resetPosition(DoubleFollowerWheelPositions wheelPositions, Pose2d pose) {
    super.resetPosition(wheelPositions.theta, wheelPositions, pose);
  }

  public Pose2d update(DoubleFollowerWheelPositions wheelPositions) {
    return super.update(wheelPositions.theta, wheelPositions);
  }

  public Pose2d update(double x, double y, Rotation2d gyroAngle) {
    return update(new DoubleFollowerWheelPositions(x, y, gyroAngle));
  }
}
