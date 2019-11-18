/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.kinematics;

import edu.wpi.first.wpilibj.Timer;
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
 * <p>There are two ways of tracking the robot's position on the field with this
 * class: one involving using encoder velocities and the other involving encoder
 * positions. It is very important that only one type of odometry is used with
 * each instantiation of this class.
 *
 * <p>Note: If you are using the encoder positions / distances method, it is important
 * that you reset your encoders to zero before using this class. Any subsequent pose
 * resets also require the encoders to be reset to zero.
 */
public class DifferentialDriveOdometry {
  private final DifferentialDriveKinematics m_kinematics;
  private Pose2d m_poseMeters;
  private double m_prevTimeSeconds = -1;

  private Rotation2d m_gyroOffset;
  private Rotation2d m_previousAngle;

  private double m_prevLeftDistance;
  private double m_prevRightDistance;

  /**
   * Constructs a DifferentialDriveOdometry object.
   *
   * @param kinematics        The differential drive kinematics for your drivetrain.
   * @param gyroAngle         The angle reported by the gyroscope.
   * @param initialPoseMeters The starting position of the robot on the field.
   */
  public DifferentialDriveOdometry(DifferentialDriveKinematics kinematics, Rotation2d gyroAngle,
                                   Pose2d initialPoseMeters) {
    m_kinematics = kinematics;
    m_poseMeters = initialPoseMeters;
    m_gyroOffset = m_poseMeters.getRotation().minus(gyroAngle);
    m_previousAngle = initialPoseMeters.getRotation();
  }

  /**
   * Constructs a DifferentialDriveOdometry object with the default pose at the origin.
   *
   * @param kinematics The differential drive kinematics for your drivetrain.
   * @param gyroAngle  The angle reported by the gyroscope.
   */
  public DifferentialDriveOdometry(DifferentialDriveKinematics kinematics, Rotation2d gyroAngle) {
    this(kinematics, gyroAngle, new Pose2d());
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>If you are using the encoder distances method instead of the velocity method,
   * you NEED to reset your encoders (to zero) when calling this method.
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
   * Updates the robot's position on the field using forward kinematics and
   * integration of the pose over time. This method takes in the current time as
   * a parameter to calculate period (difference between two timestamps). The
   * period is used to calculate the change in distance from a velocity. This
   * also takes in an angle parameter which is used instead of the
   * angular rate that is calculated from forward kinematics.
   *
   * @param currentTimeSeconds The current time in seconds.
   * @param gyroAngle          The angle reported by the gyroscope.
   * @param wheelSpeeds        The current wheel speeds.
   * @return The new pose of the robot.
   */
  public Pose2d updateWithTime(double currentTimeSeconds, Rotation2d gyroAngle,
                               DifferentialDriveWheelSpeeds wheelSpeeds) {
    double period = m_prevTimeSeconds >= 0 ? currentTimeSeconds - m_prevTimeSeconds : 0.0;
    m_prevTimeSeconds = currentTimeSeconds;

    var angle = gyroAngle.plus(m_gyroOffset);

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

  /**
   * Updates the robot's position on the field using forward kinematics and
   * integration of the pose over time. This method automatically calculates the
   * current time to calculate period (difference between two timestamps). The
   * period is used to calculate the change in distance from a velocity. This
   * also takes in an angle parameter which is used instead of the
   * angular rate that is calculated from forward kinematics.
   *
   * @param gyroAngle   The angle reported by the gyroscope.
   * @param wheelSpeeds The current wheel speeds.
   * @return The new pose of the robot.
   */
  public Pose2d update(Rotation2d gyroAngle,
                       DifferentialDriveWheelSpeeds wheelSpeeds) {
    return updateWithTime(Timer.getFPGATimestamp(),
        gyroAngle, wheelSpeeds);
  }
}
