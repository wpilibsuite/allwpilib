// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory.constraint;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;

/**
 * A class that enforces constraints on the mecanum drive kinematics. This can be used to ensure
 * that the trajectory is constructed so that the commanded velocities for all 4 wheels of the
 * drivetrain stay below a certain limit.
 */
public class MecanumDriveKinematicsConstraint implements TrajectoryConstraint {
  private final double m_maxSpeed;
  private final MecanumDriveKinematics m_kinematics;

  /**
   * Constructs a mecanum drive kinematics constraint.
   *
   * @param kinematics Mecanum drive kinematics.
   * @param maxSpeed The max speed that a side of the robot can travel at in m/s.
   */
  public MecanumDriveKinematicsConstraint(
      final MecanumDriveKinematics kinematics, double maxSpeed) {
    m_maxSpeed = maxSpeed;
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

    // Create an object to represent the current chassis speeds.
    var chassisSpeeds = new ChassisSpeeds(xdVelocity, ydVelocity, velocity * curvature);

    // Get the wheel speeds and normalize them to within the max velocity.
    var wheelSpeeds = m_kinematics.toWheelSpeeds(chassisSpeeds).desaturate(m_maxSpeed);

    // Convert normalized wheel speeds back to chassis speeds
    var normSpeeds = m_kinematics.toChassisSpeeds(wheelSpeeds);

    // Return the new linear chassis speed.
    return Math.hypot(normSpeeds.vx, normSpeeds.vy);
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
    return new MinMax();
  }
}
