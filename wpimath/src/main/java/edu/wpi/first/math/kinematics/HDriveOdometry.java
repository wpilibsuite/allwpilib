package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUsageId;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Twist2d;

/**
 * Class for H-drive odometry. Odometry allows you to track the robot's position on the
 * field over the course of a match using readings from 3 encoders and a gyroscope.
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 *
 * <p>It is important that you reset your encoders to zero before using this class. Any subsequent
 * pose resets also require the encoders to be reset to zero.
 */
public class HDriveOdometry {
  private Pose2d m_poseMeters;

  private Rotation2d m_gyroOffset;
  private Rotation2d m_previousAngle;

  private double m_prevLeftDistance;
  private double m_prevRightDistance;
  private double m_prevLateralDistance;

  /**
   * Constructs a HDriveOdometry object.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftDistanceMeters The distance traveled by the left encoder.
   * @param rightDistanceMeters The distance traveled by the right encoder.
   * @param lateralDistanceMeters The distance traveled by the lateral encoder.
   * @param initialPoseMeters The starting position of the robot on the field.
   */
  public HDriveOdometry(
      Rotation2d gyroAngle,
      double leftDistanceMeters,
      double rightDistanceMeters,
      double lateralDistanceMeters,
      Pose2d initialPoseMeters) {
    m_poseMeters = initialPoseMeters;
    m_gyroOffset = m_poseMeters.getRotation().minus(gyroAngle);
    m_previousAngle = initialPoseMeters.getRotation();

    m_prevLeftDistance = leftDistanceMeters;
    m_prevRightDistance = rightDistanceMeters;
    m_prevLateralDistance = lateralDistanceMeters;

    MathSharedStore.reportUsage(MathUsageId.kOdometry_HDrive, 1);
  }

  /**
   * Constructs a HDriveOdometry object.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftDistanceMeters The distance traveled by the left encoder.
   * @param rightDistanceMeters The distance traveled by the right encoder.
   * @param lateralDistanceMeters The distance traveled by the lateral encoder.
   */
  public HDriveOdometry(
      Rotation2d gyroAngle, double leftDistanceMeters, double rightDistanceMeters, double lateralDistanceMeters) {
    this(gyroAngle, leftDistanceMeters, rightDistanceMeters, lateralDistanceMeters, new Pose2d());
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftDistanceMeters The distance traveled by the left encoder.
   * @param rightDistanceMeters The distance traveled by the right encoder.
   * @param lateralDistanceMeters The distance traveled by the lateral encoder.
   * @param poseMeters The position on the field that your robot is at.
   */
  public void resetPosition(
      Rotation2d gyroAngle,
      double leftDistanceMeters,
      double rightDistanceMeters,
      double lateralDistanceMeters,
      Pose2d poseMeters) {
    m_poseMeters = poseMeters;
    m_previousAngle = poseMeters.getRotation();
    m_gyroOffset = m_poseMeters.getRotation().minus(gyroAngle);

    m_prevLeftDistance = leftDistanceMeters;
    m_prevRightDistance = rightDistanceMeters;
    m_prevLateralDistance = lateralDistanceMeters;
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
   * Updates the robot position on the field using distance measurements from encoders. This method
   * is more numerically accurate than using velocities to integrate the pose and is also
   * advantageous for teams that are using lower CPR encoders.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftDistanceMeters The distance traveled by the left encoder.
   * @param rightDistanceMeters The distance traveled by the right encoder.
   * @param lateralDistanceMeters The distance traveled by the lateral encoder.
   * @return The new pose of the robot.
   */
  public Pose2d update(
      Rotation2d gyroAngle, double leftDistanceMeters, double rightDistanceMeters, double lateralDistanceMeters) {
    double deltaLeftDistance = leftDistanceMeters - m_prevLeftDistance;
    double deltaRightDistance = rightDistanceMeters - m_prevRightDistance;
    double deltaLateralDistance = lateralDistanceMeters - m_prevLateralDistance;

    m_prevLeftDistance = leftDistanceMeters;
    m_prevRightDistance = rightDistanceMeters;
    m_prevLateralDistance = lateralDistanceMeters;
    
    double averageDeltaDistance = (deltaLeftDistance + deltaRightDistance) / 2.0;
    var angle = gyroAngle.plus(m_gyroOffset);

    var newPose =
        m_poseMeters.exp(
            new Twist2d(averageDeltaDistance, deltaLateralDistance, angle.minus(m_previousAngle).getRadians()));

    m_previousAngle = angle;

    m_poseMeters = new Pose2d(newPose.getTranslation(), angle);
    return m_poseMeters;
  }
}

