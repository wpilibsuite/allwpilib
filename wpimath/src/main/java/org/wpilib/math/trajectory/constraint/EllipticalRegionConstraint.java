// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.constraint;

import org.wpilib.math.geometry.Ellipse2d;
import org.wpilib.math.geometry.Pose2d;

/** Enforces a particular constraint only within an elliptical region. */
public class EllipticalRegionConstraint implements TrajectoryConstraint {
  private final Ellipse2d m_ellipse;
  private final TrajectoryConstraint m_constraint;

  /**
   * Constructs a new EllipticalRegionConstraint.
   *
   * @param ellipse The ellipse in which to enforce the constraint.
   * @param constraint The constraint to enforce when the robot is within the region.
   */
  public EllipticalRegionConstraint(Ellipse2d ellipse, TrajectoryConstraint constraint) {
    m_ellipse = ellipse;
    m_constraint = constraint;
  }

  @Override
  public double getMaxVelocity(Pose2d pose, double curvature, double velocity) {
    if (m_ellipse.contains(pose.getTranslation())) {
      return m_constraint.getMaxVelocity(pose, curvature, velocity);
    } else {
      return Double.POSITIVE_INFINITY;
    }
  }

  @Override
  public MinMax getMinMaxAcceleration(Pose2d pose, double curvature, double velocity) {
    if (m_ellipse.contains(pose.getTranslation())) {
      return m_constraint.getMinMaxAcceleration(pose, curvature, velocity);
    } else {
      return new MinMax();
    }
  }
}
