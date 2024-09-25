// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory.constraint;

import edu.wpi.first.math.geometry.Ellipse2d;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;

/** Enforces a particular constraint only within an elliptical region. */
public class EllipticalRegionConstraint implements TrajectoryConstraint {
  private final Ellipse2d m_ellipse;
  private final TrajectoryConstraint m_constraint;

  /**
   * Constructs a new EllipticalRegionConstraint.
   *
   * @param center The center of the ellipse in which to enforce the constraint.
   * @param xWidth The width of the ellipse in which to enforce the constraint.
   * @param yWidth The height of the ellipse in which to enforce the constraint.
   * @param rotation The rotation to apply to all radii around the origin.
   * @param constraint The constraint to enforce when the robot is within the region.
   * @deprecated Use constructor taking Ellipse2d instead.
   */
  @Deprecated(since = "2025", forRemoval = true)
  public EllipticalRegionConstraint(
      Translation2d center,
      double xWidth,
      double yWidth,
      Rotation2d rotation,
      TrajectoryConstraint constraint) {
    this(new Ellipse2d(new Pose2d(center, rotation), xWidth / 2.0, yWidth / 2.0), constraint);
  }

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
  public double getMaxVelocityMetersPerSecond(
      Pose2d poseMeters, double curvatureRadPerMeter, double velocityMetersPerSecond) {
    if (m_ellipse.contains(poseMeters.getTranslation())) {
      return m_constraint.getMaxVelocityMetersPerSecond(
          poseMeters, curvatureRadPerMeter, velocityMetersPerSecond);
    } else {
      return Double.POSITIVE_INFINITY;
    }
  }

  @Override
  public MinMax getMinMaxAccelerationMetersPerSecondSq(
      Pose2d poseMeters, double curvatureRadPerMeter, double velocityMetersPerSecond) {
    if (m_ellipse.contains(poseMeters.getTranslation())) {
      return m_constraint.getMinMaxAccelerationMetersPerSecondSq(
          poseMeters, curvatureRadPerMeter, velocityMetersPerSecond);
    } else {
      return new MinMax();
    }
  }
}
