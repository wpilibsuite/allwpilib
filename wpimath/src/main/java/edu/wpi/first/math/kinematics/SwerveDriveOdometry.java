// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUsageId;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;

/**
 * Class for swerve drive odometry. Odometry allows you to track the robot's position on the field
 * over a course of a match using readings from your swerve drive encoders and swerve azimuth
 * encoders.
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 */
public class SwerveDriveOdometry {
  private final SwerveDriveKinematics m_kinematics;
  private Pose2d m_poseMeters;

  private Rotation2d m_gyroOffset;
  private Rotation2d m_previousAngle;
  private final int m_numModules;
  private SwerveModulePosition[] m_previousModulePositions;

  /**
   * Constructs a SwerveDriveOdometry object.
   *
   * @param kinematics The swerve drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param modulePositions The wheel positions reported by each module.
   * @param initialPose The starting position of the robot on the field.
   */
  public SwerveDriveOdometry(
      SwerveDriveKinematics kinematics,
      Rotation2d gyroAngle,
      SwerveModulePosition[] modulePositions,
      Pose2d initialPose) {
    m_kinematics = kinematics;
    m_poseMeters = initialPose;
    m_gyroOffset = m_poseMeters.getRotation().minus(gyroAngle);
    m_previousAngle = initialPose.getRotation();
    m_numModules = modulePositions.length;

    m_previousModulePositions = new SwerveModulePosition[m_numModules];
    for (int index = 0; index < m_numModules; index++) {
      m_previousModulePositions[index] =
          new SwerveModulePosition(
              modulePositions[index].distanceMeters, modulePositions[index].angle);
    }

    MathSharedStore.reportUsage(MathUsageId.kOdometry_SwerveDrive, 1);
  }

  /**
   * Constructs a SwerveDriveOdometry object with the default pose at the origin.
   *
   * @param kinematics The swerve drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param modulePositions The wheel positions reported by each module.
   */
  public SwerveDriveOdometry(
      SwerveDriveKinematics kinematics,
      Rotation2d gyroAngle,
      SwerveModulePosition[] modulePositions) {
    this(kinematics, gyroAngle, modulePositions, new Pose2d());
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * <p>Similarly, module positions do not need to be reset in user code.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param modulePositions The wheel positions reported by each module.,
   * @param pose The position on the field that your robot is at.
   */
  public void resetPosition(
      Rotation2d gyroAngle, SwerveModulePosition[] modulePositions, Pose2d pose) {
    if (modulePositions.length != m_numModules) {
      throw new IllegalArgumentException(
          "Number of modules is not consistent with number of wheel locations provided in "
              + "constructor");
    }

    m_poseMeters = pose;
    m_previousAngle = pose.getRotation();
    m_gyroOffset = m_poseMeters.getRotation().minus(gyroAngle);
    for (int index = 0; index < m_numModules; index++) {
      m_previousModulePositions[index] =
          new SwerveModulePosition(
              modulePositions[index].distanceMeters, modulePositions[index].angle);
    }
  }

  /**
   * Returns the position of the robot on the field.
   *
   * @return The pose of the robot (x and y are in meters).
   */
  public Pose2d getPoseMeters() {
    return m_poseMeters;
  }

  /**
   * Updates the robot's position on the field using forward kinematics and integration of the pose
   * over time. This method automatically calculates the current time to calculate period
   * (difference between two timestamps). The period is used to calculate the change in distance
   * from a velocity. This also takes in an angle parameter which is used instead of the angular
   * rate that is calculated from forward kinematics.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param modulePositions The current position of all swerve modules. Please provide the positions
   *     in the same order in which you instantiated your SwerveDriveKinematics.
   * @return The new pose of the robot.
   */
  public Pose2d update(Rotation2d gyroAngle, SwerveModulePosition[] modulePositions) {
    if (modulePositions.length != m_numModules) {
      throw new IllegalArgumentException(
          "Number of modules is not consistent with number of wheel locations provided in "
              + "constructor");
    }

    var moduleDeltas = new SwerveModulePosition[m_numModules];
    for (int index = 0; index < m_numModules; index++) {
      var current = modulePositions[index];
      var previous = m_previousModulePositions[index];

      moduleDeltas[index] =
          new SwerveModulePosition(current.distanceMeters - previous.distanceMeters, current.angle);
      previous.distanceMeters = current.distanceMeters;
    }

    var angle = gyroAngle.plus(m_gyroOffset);

    var twist = m_kinematics.toTwist2d(moduleDeltas);
    twist.dtheta = angle.minus(m_previousAngle).getRadians();

    var newPose = m_poseMeters.exp(twist);

    m_previousAngle = angle;
    m_poseMeters = new Pose2d(newPose.getTranslation(), angle);

    return m_poseMeters;
  }
}
