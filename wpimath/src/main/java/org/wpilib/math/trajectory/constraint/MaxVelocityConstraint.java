// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.constraint;

import org.wpilib.math.geometry.Pose2d;

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
   * @param maxVelocity The max velocity in m/s.
   */
  public MaxVelocityConstraint(double maxVelocity) {
    m_maxVelocity = maxVelocity;
  }

  @Override
  public double getMaxVelocity(Pose2d pose, double curvature, double velocity) {
    return m_maxVelocity;
  }

  @Override
  public TrajectoryConstraint.MinMax getMinMaxAcceleration(
      Pose2d pose, double curvature, double velocity) {
    return new MinMax();
  }
}
