/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.trajectory.constraint;

import edu.wpi.first.wpilibj.geometry.Pose2d;

/**
 * Represents a constraint that enforces a max velocity. This can be composed with the
 * {@link EllipticalRegionConstraint} or {@link RectangularRegionConstraint} to enforce
 * a max velocity in a region.
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
  public double getMaxVelocityMetersPerSecond(Pose2d poseMeters, double curvatureRadPerMeter,
                                              double velocityMetersPerSecond) {
    return m_maxVelocity;
  }

  @Override
  public TrajectoryConstraint.MinMax getMinMaxAccelerationMetersPerSecondSq(
      Pose2d poseMeters, double curvatureRadPerMeter, double velocityMetersPerSecond) {
    return new MinMax();
  }
}
