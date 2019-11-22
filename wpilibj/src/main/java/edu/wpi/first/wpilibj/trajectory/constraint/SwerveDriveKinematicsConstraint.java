/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.trajectory.constraint;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.SwerveDriveKinematics;

/**
 * A class that enforces constraints on the swerve drive kinematics.
 * This can be used to ensure that the trajectory is constructed so that the
 * commanded velocities for all 4 wheels of the drivetrain stay below a certain
 * limit.
 */
public class SwerveDriveKinematicsConstraint implements TrajectoryConstraint {
  private final double m_maxSpeedMetersPerSecond;
  private final SwerveDriveKinematics m_kinematics;

  /**
   * Constructs a mecanum drive dynamics constraint.
   *
   * @param maxSpeedMetersPerSecond The max speed that a side of the robot can travel at.
   */
  public SwerveDriveKinematicsConstraint(final SwerveDriveKinematics kinematics,
                                               double maxSpeedMetersPerSecond) {
    m_maxSpeedMetersPerSecond = maxSpeedMetersPerSecond;
    m_kinematics = kinematics;
  }


  /**
   * Returns the max velocity given the current pose and curvature.
   *
   * @param poseMeters              The pose at the current point in the trajectory.
   * @param curvatureRadPerMeter    The curvature at the current point in the trajectory.
   * @param velocityMetersPerSecond The velocity at the current point in the trajectory before
   *                                constraints are applied.
   * @return The absolute maximum velocity.
   */
  @Override
  public double getMaxVelocityMetersPerSecond(Pose2d poseMeters, double curvatureRadPerMeter,
                                              double velocityMetersPerSecond) {
    // Represents the velocity of the chassis in the x direction
    var xdVelocity = velocityMetersPerSecond * poseMeters.getRotation().getCos();

    // Represents the velocity of the chassis in the y direction
    var ydVelocity = velocityMetersPerSecond * poseMeters.getRotation().getSin();

    // Create an object to represent the current chassis speeds.
    var chassisSpeeds = new ChassisSpeeds(xdVelocity,
        ydVelocity, velocityMetersPerSecond * curvatureRadPerMeter);

    // Get the wheel speeds and normalize them to within the max velocity.
    var wheelSpeeds = m_kinematics.toSwerveModuleStates(chassisSpeeds);
    SwerveDriveKinematics.normalizeWheelSpeeds(wheelSpeeds, m_maxSpeedMetersPerSecond);

    // Convert normalized wheel speeds back to chassis speeds
    var normSpeeds = m_kinematics.toChassisSpeeds(wheelSpeeds);

    // Return the new linear chassis speed.
    return Math.hypot(normSpeeds.vxMetersPerSecond, normSpeeds.vyMetersPerSecond);
  }

  /**
   * Returns the minimum and maximum allowable acceleration for the trajectory
   * given pose, curvature, and speed.
   *
   * @param poseMeters              The pose at the current point in the trajectory.
   * @param curvatureRadPerMeter    The curvature at the current point in the trajectory.
   * @param velocityMetersPerSecond The speed at the current point in the trajectory.
   * @return The min and max acceleration bounds.
   */
  @Override
  public MinMax getMinMaxAccelerationMetersPerSecondSq(Pose2d poseMeters,
                                                       double curvatureRadPerMeter,
                                                       double velocityMetersPerSecond) {
    return new MinMax();
  }
}
