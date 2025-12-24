// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.constraint;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;

/**
 * A class that enforces constraints on the differential drive kinematics. This can be used to
 * ensure that the trajectory is constructed so that the commanded velocities for both sides of the
 * drivetrain stay below a certain limit.
 */
public class DifferentialDriveKinematicsConstraint implements TrajectoryConstraint {
  private final double m_maxVelocity;
  private final DifferentialDriveKinematics m_kinematics;

  /**
   * Constructs a differential drive dynamics constraint.
   *
   * @param kinematics A kinematics component describing the drive geometry.
   * @param maxVelocity The max velocity that a side of the robot can travel at in m/s.
   */
  public DifferentialDriveKinematicsConstraint(
      final DifferentialDriveKinematics kinematics, double maxVelocity) {
    m_maxVelocity = maxVelocity;
    m_kinematics = kinematics;
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
    // Create an object to represent the current chassis velocities.
    var chassisVelocities = new ChassisVelocities(velocity, 0, velocity * curvature);

    // Get the wheel velocities and normalize them to within the max velocity.
    var wheelVelocities = m_kinematics.toWheelVelocities(chassisVelocities);
    wheelVelocities.desaturate(m_maxVelocity);

    // Return the new linear chassis velocity.
    return m_kinematics.toChassisVelocities(wheelVelocities).vx;
  }

  /**
   * Returns the minimum and maximum allowable acceleration for the trajectory given pose,
   * curvature, and velocity.
   *
   * @param pose The pose at the current point in the trajectory.
   * @param curvature The curvature at the current point in the trajectory in rad/m.
   * @param velocity The velocity at the current point in the trajectory in m/s.
   * @return The min and max acceleration bounds.
   */
  @Override
  public MinMax getMinMaxAcceleration(Pose2d pose, double curvature, double velocity) {
    return new MinMax();
  }
}
