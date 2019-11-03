/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.trajectory.constraint;

import edu.wpi.first.wpilibj.geometry.Pose2d;

/**
 * An interface for defining user-defined velocity and acceleration constraints
 * while generating trajectories.
 */
public interface TrajectoryConstraint {

  /**
   * Returns the max velocity given the current pose and curvature.
   *
   * @param poseMeters              The pose at the current point in the trajectory.
   * @param curvatureRadPerMeter    The curvature at the current point in the trajectory.
   * @param velocityMetersPerSecond The velocity at the current point in the trajectory before
   *                                constraints are applied.
   * @return The absolute maximum velocity.
   */
  double getMaxVelocityMetersPerSecond(Pose2d poseMeters, double curvatureRadPerMeter,
                                       double velocityMetersPerSecond);

  /**
   * Returns the minimum and maximum allowable acceleration for the trajectory
   * given pose, curvature, and speed.
   *
   * @param poseMeters              The pose at the current point in the trajectory.
   * @param curvatureRadPerMeter    The curvature at the current point in the trajectory.
   * @param velocityMetersPerSecond The speed at the current point in the trajectory.
   * @return The min and max acceleration bounds.
   */
  MinMax getMinMaxAccelerationMetersPerSecondSq(Pose2d poseMeters, double curvatureRadPerMeter,
                                                double velocityMetersPerSecond);

  /**
   * Represents a minimum and maximum acceleration.
   */
  @SuppressWarnings("MemberName")
  class MinMax {
    public double minAccelerationMetersPerSecondSq = -Double.MAX_VALUE;
    public double maxAccelerationMetersPerSecondSq = +Double.MAX_VALUE;

    /**
     * Constructs a MinMax.
     *
     * @param minAccelerationMetersPerSecondSq The minimum acceleration.
     * @param maxAccelerationMetersPerSecondSq The maximum acceleration.
     */
    public MinMax(double minAccelerationMetersPerSecondSq,
                  double maxAccelerationMetersPerSecondSq) {
      this.minAccelerationMetersPerSecondSq = minAccelerationMetersPerSecondSq;
      this.maxAccelerationMetersPerSecondSq = maxAccelerationMetersPerSecondSq;
    }

    /**
     * Constructs a MinMax with default values.
     */
    public MinMax() {
    }
  }
}
