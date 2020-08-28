/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.kinematics;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUsageId;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Twist2d;

/**
 * Class for differential drive odometry. Odometry allows you to track the
 * robot's position on the field over the course of a match using readings from
 * 2 encoders and a gyroscope.
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 *
 * <p>It is important that you reset your encoders to zero before using this class.
 * Any subsequent pose resets also require the encoders to be reset to zero.
 */
public class DifferentialDriveOdometry {
  private Pose2d m_poseMeters;

  private Rotation2d m_gyroOffset;
  private Rotation2d m_previousAngle;

  private double m_prevLeftDistance;
  private double m_prevRightDistance;

  /**
   * Constructs a DifferentialDriveOdometry object.
   *
   * @param gyroAngle         The angle reported by the gyroscope.
   * @param initialPoseMeters The starting position of the robot on the field.
   */
  public DifferentialDriveOdometry(Rotation2d gyroAngle,
                                   Pose2d initialPoseMeters) {
    m_poseMeters = initialPoseMeters;
    m_gyroOffset = m_poseMeters.getRotation().minus(gyroAngle);
    m_previousAngle = initialPoseMeters.getRotation();
    MathSharedStore.reportUsage(MathUsageId.kOdometry_DifferentialDrive, 1);
  }

  /**
   * Constructs a DifferentialDriveOdometry object with the default pose at the origin.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   */
  public DifferentialDriveOdometry(Rotation2d gyroAngle) {
    this(gyroAngle, new Pose2d());
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>You NEED to reset your encoders (to zero) when calling this method.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code.
   * The library automatically takes care of offsetting the gyro angle.
   *
   * @param poseMeters The position on the field that your robot is at.
   * @param gyroAngle  The angle reported by the gyroscope.
   */
  public void resetPosition(Pose2d poseMeters, Rotation2d gyroAngle) {
    m_poseMeters = poseMeters;
    m_previousAngle = poseMeters.getRotation();
    m_gyroOffset = m_poseMeters.getRotation().minus(gyroAngle);

    m_prevLeftDistance = 0.0;
    m_prevRightDistance = 0.0;
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
   * Updates the robot position on the field using distance measurements from encoders. This
   * method is more numerically accurate than using velocities to integrate the pose and
   * is also advantageous for teams that are using lower CPR encoders.
   *
   * @param gyroAngle           The angle reported by the gyroscope.
   * @param leftDistanceMeters  The distance traveled by the left encoder.
   * @param rightDistanceMeters The distance traveled by the right encoder.
   * @return The new pose of the robot.
   */
  public Pose2d update(Rotation2d gyroAngle, double leftDistanceMeters,
                       double rightDistanceMeters) {
    double deltaLeftDistance = leftDistanceMeters - m_prevLeftDistance;
    double deltaRightDistance = rightDistanceMeters - m_prevRightDistance;

    m_prevLeftDistance = leftDistanceMeters;
    m_prevRightDistance = rightDistanceMeters;

    double averageDeltaDistance = (deltaLeftDistance + deltaRightDistance) / 2.0;
    var angle = gyroAngle.plus(m_gyroOffset);

    var newPose = m_poseMeters.exp(
        new Twist2d(averageDeltaDistance, 0.0, angle.minus(m_previousAngle).getRadians()));

    m_previousAngle = angle;

    m_poseMeters = new Pose2d(newPose.getTranslation(), angle);
    return m_poseMeters;
  }
}
