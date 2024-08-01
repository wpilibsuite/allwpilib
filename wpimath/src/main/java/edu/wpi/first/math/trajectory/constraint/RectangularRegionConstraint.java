// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory.constraint;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rectangle2d;
import edu.wpi.first.math.geometry.Translation2d;

/** Enforces a particular constraint only within a rectangular region. */
public class RectangularRegionConstraint implements TrajectoryConstraint {
  private final Rectangle2d m_rectangle;
  private final TrajectoryConstraint m_constraint;

  /**
   * Constructs a new RectangularRegionConstraint.
   *
   * @param bottomLeftPoint The bottom left point of the rectangular region in which to enforce the
   *     constraint.
   * @param topRightPoint The top right point of the rectangular region in which to enforce the
   *     constraint.
   * @param constraint The constraint to enforce when the robot is within the region.
   * @deprecated Use constructor taking Rectangle2d instead.
   */
  @Deprecated(since = "2025", forRemoval = true)
  public RectangularRegionConstraint(
      Translation2d bottomLeftPoint, Translation2d topRightPoint, TrajectoryConstraint constraint) {
    this(new Rectangle2d(bottomLeftPoint, topRightPoint), constraint);
  }

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
  public double getMaxVelocityMetersPerSecond(
      Pose2d poseMeters, double curvatureRadPerMeter, double velocityMetersPerSecond) {
    if (m_rectangle.contains(poseMeters.getTranslation())) {
      return m_constraint.getMaxVelocityMetersPerSecond(
          poseMeters, curvatureRadPerMeter, velocityMetersPerSecond);
    } else {
      return Double.POSITIVE_INFINITY;
    }
  }

  @Override
  public MinMax getMinMaxAccelerationMetersPerSecondSq(
      Pose2d poseMeters, double curvatureRadPerMeter, double velocityMetersPerSecond) {
    if (m_rectangle.contains(poseMeters.getTranslation())) {
      return m_constraint.getMinMaxAccelerationMetersPerSecondSq(
          poseMeters, curvatureRadPerMeter, velocityMetersPerSecond);
    } else {
      return new MinMax();
    }
  }
}
