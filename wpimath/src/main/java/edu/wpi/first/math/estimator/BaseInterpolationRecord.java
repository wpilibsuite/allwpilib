package edu.wpi.first.math.estimator;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.interpolation.Interpolatable;

public abstract class BaseInterpolationRecord implements Interpolatable<BaseInterpolationRecord> {
  // The pose observed given the current sensor inputs and the previous pose.
  protected final Pose2d poseMeters;

  // The current gyro angle.
  protected final Rotation2d gyroAngle;

  public BaseInterpolationRecord(Pose2d poseMeters, Rotation2d gyro) {
    this.poseMeters = poseMeters;
    this.gyroAngle = gyro;
  }

  public abstract BaseInterpolationRecord interpolate(BaseInterpolationRecord endValue, double t);
}
