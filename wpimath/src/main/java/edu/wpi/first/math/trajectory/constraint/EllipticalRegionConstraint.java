// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory.constraint;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;

/** Enforces a particular constraint only within an elliptical region. */
public class EllipticalRegionConstraint implements TrajectoryConstraint {
  private final Translation2d m_center;
  private final Translation2d m_radii;
  private final TrajectoryConstraint m_constraint;

  /**
   * Constructs a new EllipticalRegionConstraint.
   *
   * @param center The center of the ellipse in which to enforce the constraint.
   * @param xWidth The width of the ellipse in which to enforce the constraint.
   * @param yWidth The height of the ellipse in which to enforce the constraint.
   * @param rotation The rotation to apply to all radii around the origin.
   * @param constraint The constraint to enforce when the robot is within the region.
   */
  public EllipticalRegionConstraint(
      Translation2d center,
      double xWidth,
      double yWidth,
      Rotation2d rotation,
      TrajectoryConstraint constraint) {
    m_center = center;
    m_radii = new Translation2d(xWidth / 2.0, yWidth / 2.0).rotateBy(rotation);
    m_constraint = constraint;
  }

  @Override
  public double getMaxVelocityMetersPerSecond(
      Pose2d poseMeters, double curvatureRadPerMeter, double velocityMetersPerSecond) {
    if (isPoseInRegion(poseMeters)) {
      return m_constraint.getMaxVelocityMetersPerSecond(
          poseMeters, curvatureRadPerMeter, velocityMetersPerSecond);
    } else {
      return Double.POSITIVE_INFINITY;
    }
  }

  @Override
  public MinMax getMinMaxAccelerationMetersPerSecondSq(
      Pose2d poseMeters, double curvatureRadPerMeter, double velocityMetersPerSecond) {
    if (isPoseInRegion(poseMeters)) {
      return m_constraint.getMinMaxAccelerationMetersPerSecondSq(
          poseMeters, curvatureRadPerMeter, velocityMetersPerSecond);
    } else {
      return new MinMax();
    }
  }

  /**
   * Returns whether the specified robot pose is within the region that the constraint is enforced
   * in.
   *
   * @param robotPose The robot pose.
   * @return Whether the robot pose is within the constraint region.
   */
  public boolean isPoseInRegion(Pose2d robotPose) {
    // The region bounded by the ellipse is given by the equation:
    //
    // (x−h)²/Rx² + (y−k)²/Ry² ≤ 1
    //
    // Multiply by Rx²Ry² for efficiency reasons:
    //
    // (x−h)²Ry² + (y−k)²Rx² ≤ Rx²Ry²
    //
    // If the inequality is satisfied, then it is inside the ellipse; otherwise
    // it is outside the ellipse.
    return Math.pow(robotPose.getX() - m_center.getX(), 2) * Math.pow(m_radii.getY(), 2)
            + Math.pow(robotPose.getY() - m_center.getY(), 2) * Math.pow(m_radii.getX(), 2)
        <= Math.pow(m_radii.getX(), 2) * Math.pow(m_radii.getY(), 2);
  }
}
