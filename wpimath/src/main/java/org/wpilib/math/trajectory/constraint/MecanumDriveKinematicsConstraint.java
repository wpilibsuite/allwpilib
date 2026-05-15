// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.constraint;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.kinematics.MecanumDriveKinematics;

/**
 * A class that enforces constraints on the mecanum drive kinematics. This can be used to ensure
 * that the trajectory is constructed so that the commanded velocities for all 4 wheels of the
 * drivetrain stay below a certain limit.
 */
public class MecanumDriveKinematicsConstraint implements TrajectoryConstraint {
  private final double m_maxVelocity;
  private final MecanumDriveKinematics m_kinematics;

  /**
   * Constructs a mecanum drive kinematics constraint.
   *
   * @param kinematics Mecanum drive kinematics.
   * @param maxVelocity The max velocity that a side of the robot can travel at in m/s.
   */
  public MecanumDriveKinematicsConstraint(
      final MecanumDriveKinematics kinematics, double maxVelocity) {
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
    // Represents the velocity of the chassis in the x direction
    var xdVelocity = velocity * pose.getRotation().getCos();

    // Represents the velocity of the chassis in the y direction
    var ydVelocity = velocity * pose.getRotation().getSin();

    // Create an object to represent the current chassis velocities.
    var chassisVelocities = new ChassisVelocities(xdVelocity, ydVelocity, velocity * curvature);

    // Get the wheel velocities and normalize them to within the max velocity.
    var wheelVelocities =
        m_kinematics.toWheelVelocities(chassisVelocities).desaturate(m_maxVelocity);

    // Convert normalized wheel velocities back to chassis velocities
    var normVelocities = m_kinematics.toChassisVelocities(wheelVelocities);

    // Return the new linear chassis velocity.
    return Math.hypot(normVelocities.vx, normVelocities.vy);
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
