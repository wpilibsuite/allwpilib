// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory.constraint;

import edu.wpi.first.math.geometry.Pose2d;

/**
 * Represents a constraint that enforces a max velocity. This can be composed with the {@link
 * EllipticalRegionConstraint} or {@link RectangularRegionConstraint} to enforce a max velocity in a
 * region.
 */
public class MaxVelocityConstraint implements TrajectoryConstraint {
  private final double m_maxVelocity;

  /**
   * Constructs a new MaxVelocityConstraint.
   *
   * @param maxVelocityMetersPerSecond The max velocity.
   */
  public MaxVelocityConstraint(double maxVelocityMetersPerSecond) {
    m_maxVelocity = maxVelocityMetersPerSecond;
  }

  @Override
  public double getMaxVelocityMetersPerSecond(
      Pose2d poseMeters, double curvatureRadPerMeter, double velocityMetersPerSecond) {
    return m_maxVelocity;
  }

  @Override
  public TrajectoryConstraint.MinMax getMinMaxAccelerationMetersPerSecondSq(
      Pose2d poseMeters, double curvatureRadPerMeter, double velocityMetersPerSecond) {
    return new MinMax();
  }
}
