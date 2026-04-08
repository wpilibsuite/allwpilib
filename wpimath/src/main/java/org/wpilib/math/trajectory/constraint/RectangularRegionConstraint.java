// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.constraint;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rectangle2d;

/** Enforces a particular constraint only within a rectangular region. */
public class RectangularRegionConstraint implements TrajectoryConstraint {
  private final Rectangle2d m_rectangle;
  private final TrajectoryConstraint m_constraint;

  /**
   * Constructs a new RectangularRegionConstraint.
   *
   * @param rectangle The rectangular region in which to enforce the constraint.
   * @param constraint The constraint to enforce when the robot is within the region.
   */
  public RectangularRegionConstraint(Rectangle2d rectangle, TrajectoryConstraint constraint) {
    m_rectangle = rectangle;
    m_constraint = constraint;
  }

  @Override
  public double getMaxVelocity(Pose2d pose, double curvature, double velocity) {
    if (m_rectangle.contains(pose.getTranslation())) {
      return m_constraint.getMaxVelocity(pose, curvature, velocity);
    } else {
      return Double.POSITIVE_INFINITY;
    }
  }

  @Override
  public MinMax getMinMaxAcceleration(Pose2d pose, double curvature, double velocity) {
    if (m_rectangle.contains(pose.getTranslation())) {
      return m_constraint.getMinMaxAcceleration(pose, curvature, velocity);
    } else {
      return new MinMax();
    }
  }
}
