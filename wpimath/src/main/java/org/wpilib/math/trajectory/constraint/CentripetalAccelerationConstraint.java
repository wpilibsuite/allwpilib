// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.constraint;

import org.wpilib.math.geometry.Pose2d;

/**
 * A constraint on the maximum absolute centripetal acceleration allowed when traversing a
 * trajectory. The centripetal acceleration of a robot is defined as the velocity squared divided by
 * the radius of curvature.
 *
 * <p>Effectively, limiting the maximum centripetal acceleration will cause the robot to slow down
 * around tight turns, making it easier to track trajectories with sharp turns.
 */
public class CentripetalAccelerationConstraint implements TrajectoryConstraint {
  private final double m_maxCentripetalAcceleration;

  /**
   * Constructs a centripetal acceleration constraint.
   *
   * @param maxCentripetalAcceleration The max centripetal acceleration in m/s².
   */
  public CentripetalAccelerationConstraint(double maxCentripetalAcceleration) {
    m_maxCentripetalAcceleration = maxCentripetalAcceleration;
  }

  /**
   * Returns the max velocity given the current pose and curvature.
   *
   * @param pose The pose at the current point in the trajectory.
   * @param curvature The curvature at the current point in the trajectory in rad/m.
   * @param velocity The velocity at the current point in the trajectory before constraints are
   *     applied in m/s.
   * @return The absolute maximum velocity.
   */
  @Override
  public double getMaxVelocity(Pose2d pose, double curvature, double velocity) {
    // ac = v²/r
    // k (curvature) = 1/r

    // therefore, ac = v²k
    // ac/k = v²
    // v = √(ac/k)

    return Math.sqrt(m_maxCentripetalAcceleration / Math.abs(curvature));
  }

  /**
   * Returns the minimum and maximum allowable acceleration for the trajectory given pose,
   * curvature, and speed.
   *
   * @param pose The pose at the current point in the trajectory.
   * @param curvature The curvature at the current point in the trajectory in rad/m.
   * @param velocity The speed at the current point in the trajectory in m/s.
   * @return The min and max acceleration bounds.
   */
  @Override
  public MinMax getMinMaxAcceleration(Pose2d pose, double curvature, double velocity) {
    // The acceleration of the robot has no impact on the centripetal acceleration
    // of the robot.
    return new MinMax();
  }
}
