/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.kinematics;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Twist2d;

/**
 * Class for mecnaum drive odometry. Odometry allows you to track the robot's
 * position on the field over a course of a match using readings from your
 * mecanum wheel encoders.
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 */
public class MecanumDriveOdometry {
  private final MecanumDriveKinematics m_kinematics;
  private Pose2d m_poseMeters;
  private double m_prevTimeSeconds = -1;

  private Rotation2d m_previousAngle;

  /**
   * Constructs a MecanumDriveOdometry object.
   *
   * @param kinematics        The mecanum drive kinematics for your drivetrain.
   * @param initialPoseMeters The starting position of the robot on the field.
   */
  public MecanumDriveOdometry(MecanumDriveKinematics kinematics, Pose2d initialPoseMeters) {
    m_kinematics = kinematics;
    m_poseMeters = initialPoseMeters;
    m_previousAngle = initialPoseMeters.getRotation();
  }

  /**
   * Constructs a MecanumDriveOdometry object with the default pose at the origin.
   *
   * @param kinematics The mecanum drive kinematics for your drivetrain.
   */
  public MecanumDriveOdometry(MecanumDriveKinematics kinematics) {
    this(kinematics, new Pose2d());
  }

  /**
   * Resets the robot's position on the field.
   *
   * @param poseMeters The position on the field that your robot is at.
   */
  public void resetPosition(Pose2d poseMeters) {
    m_poseMeters = poseMeters;
    m_previousAngle = poseMeters.getRotation();
  }

  /**
   * Updates the robot's position on the field using forward kinematics and
   * integration of the pose over time. This method takes in the current time as
   * a parameter to calculate period (difference between two timestamps). The
   * period is used to calculate the change in distance from a velocity. This
   * also takes in an angle parameter which is used instead of the
   * angular rate that is calculated from forward kinematics.
   *
   * @param currentTimeSeconds The current time in seconds.
   * @param angle              The angle of the robot.
   * @param wheelSpeeds        The current wheel speeds.
   * @return The new pose of the robot.
   */
  public Pose2d updateWithTime(double currentTimeSeconds, Rotation2d angle,
                               MecanumDriveWheelSpeeds wheelSpeeds) {
    double period = m_prevTimeSeconds >= 0 ? currentTimeSeconds - m_prevTimeSeconds : 0.0;
    m_prevTimeSeconds = currentTimeSeconds;

    var chassisState = m_kinematics.toChassisSpeeds(wheelSpeeds);
    var newPose = m_poseMeters.exp(
        new Twist2d(chassisState.vxMetersPerSecond * period,
            chassisState.vyMetersPerSecond * period,
            angle.minus(m_previousAngle).getRadians()));

    m_previousAngle = angle;
    m_poseMeters = new Pose2d(newPose.getTranslation(), angle);
    return m_poseMeters;
  }

  /**
   * Updates the robot's position on the field using forward kinematics and
   * integration of the pose over time. This method automatically calculates the
   * current time to calculate period (difference between two timestamps). The
   * period is used to calculate the change in distance from a velocity. This
   * also takes in an angle parameter which is used instead of the
   * angular rate that is calculated from forward kinematics.
   *
   * @param angle       THe angle of the robot.
   * @param wheelSpeeds The current wheel speeds.
   * @return The new pose of the robot.
   */
  public Pose2d update(Rotation2d angle,
                       MecanumDriveWheelSpeeds wheelSpeeds) {
    return updateWithTime(System.currentTimeMillis() / 1000.0, angle,
        wheelSpeeds);
  }
}
