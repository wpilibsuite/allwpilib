// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.constraint;

import org.wpilib.math.geometry.Pose2d;

/**
 * An interface for defining user-defined velocity and acceleration constraints while generating
 * trajectories.
 */
public interface TrajectoryConstraint {
  /**
   * Returns the max velocity given the current pose and curvature.
   *
   * @param pose The pose at the current point in the trajectory.
   * @param curvature The curvature at the current point in the trajectory rad/m.
   * @param velocity The velocity at the current point in the trajectory before constraints are
   *     applied in m/s.
   * @return The absolute maximum velocity.
   */
  double getMaxVelocity(Pose2d pose, double curvature, double velocity);

  /**
   * Returns the minimum and maximum allowable acceleration for the trajectory given pose,
   * curvature, and velocity.
   *
   * @param pose The pose at the current point in the trajectory.
   * @param curvature The curvature at the current point in the trajectory rad/m.
   * @param velocity The velocity at the current point in the trajectory in m/s.
   * @return The min and max acceleration bounds.
   */
  MinMax getMinMaxAcceleration(Pose2d pose, double curvature, double velocity);

  /** Represents a minimum and maximum acceleration. */
  class MinMax {
    /** The minimum acceleration. */
    public double minAcceleration = -Double.MAX_VALUE;

    /** The maximum acceleration. */
    public double maxAcceleration = Double.MAX_VALUE;

    /**
     * Constructs a MinMax.
     *
     * @param minAcceleration The minimum acceleration in m/s².
     * @param maxAcceleration The maximum acceleration in m/s².
     */
    public MinMax(double minAcceleration, double maxAcceleration) {
      this.minAcceleration = minAcceleration;
      this.maxAcceleration = maxAcceleration;
    }

    /** Constructs a MinMax with default values. */
    public MinMax() {}
  }
}
