// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.geometry.Translation2d;

public final class ComputerVisionUtil {
  private ComputerVisionUtil() {
    throw new AssertionError("utility class");
  }

  /**
   * Algorithm from https://docs.limelightvision.io/en/latest/cs_estimating_distance.html Estimates
   * range to a target using the target's elevation. This method can produce more stable results
   * than SolvePNP when well tuned, if the full 6d robot pose is not required. Note that this method
   * requires the camera to have 0 roll (not be skewed clockwise or CCW relative to the floor), and
   * for there to exist a height differential between goal and camera. The larger this differential,
   * the more accurate the distance estimate will be.
   *
   * <p>Units can be converted using the {@link edu.wpi.first.math.util.Units} class.
   *
   * @param cameraHeightMeters The physical height of the camera off the floor in meters.
   * @param targetHeightMeters The physical height of the target off the floor in meters. This
   *     should be the height of whatever is being targeted (i.e. if the targeting region is set to
   *     top, this should be the height of the top of the target).
   * @param cameraPitchRadians The pitch of the camera from the horizontal plane in radians.
   *     Positive values up.
   * @param targetPitchRadians The pitch of the target in the camera's lens in radians. Positive
   *     values up.
   * @param targetYawRadians The yaw of the target in the camera's lens in radians.
   * @return The estimated distance to the target in meters.
   */
  public static double calculateDistanceToTarget(
      double cameraHeightMeters,
      double targetHeightMeters,
      double cameraPitchRadians,
      double targetPitchRadians,
      double targetYawRadians) {
    return (targetHeightMeters - cameraHeightMeters)
        / (Math.tan(cameraPitchRadians + targetPitchRadians) * Math.cos(targetYawRadians));
  }

  /**
   * Estimate the position of the robot in the field.
   *
   * @param cameraHeightMeters The physical height of the camera off the floor in meters.
   * @param targetHeightMeters The physical height of the target off the floor in meters. This
   *     should be the height of whatever is being targeted (i.e. if the targeting region is set to
   *     top, this should be the height of the top of the target).
   * @param cameraPitchRadians The pitch of the camera from the horizontal plane in radians.
   *     Positive values up.
   * @param targetPitchRadians The pitch of the target in the camera's lens in radians. Positive
   *     values up.
   * @param targetYaw The observed yaw of the target. Note that this *must* be CCW-positive, and
   *     Photon returns CW-positive.
   * @param gyroAngle The current robot gyro angle, likely from odometry.
   * @param fieldToTarget A Pose2d representing the target position in the field coordinate system.
   * @param cameraToRobot The position of the robot relative to the camera. If the camera was
   *     mounted 3 inches behind the "origin" (usually physical center) of the robot, this would be
   *     Transform2d(3 inches, 0 inches, 0 degrees).
   * @return The position of the robot in the field.
   */
  public static Pose2d estimateFieldToRobot(
      double cameraHeightMeters,
      double targetHeightMeters,
      double cameraPitchRadians,
      double targetPitchRadians,
      Rotation2d targetYaw,
      Rotation2d gyroAngle,
      Pose2d fieldToTarget,
      Transform2d cameraToRobot) {
    return estimateFieldToRobot(
        estimateCameraToTarget(
            new Translation2d(
                calculateDistanceToTarget(
                    cameraHeightMeters,
                    targetHeightMeters,
                    cameraPitchRadians,
                    targetPitchRadians,
                    targetYaw.getRadians()),
                targetYaw),
            fieldToTarget,
            gyroAngle),
        fieldToTarget,
        cameraToRobot);
  }

  /**
   * Estimates the pose of the robot in the field coordinate system, given the position of the
   * target relative to the camera, the target relative to the field, and the robot relative to the
   * camera.
   *
   * @param cameraToTarget The position of the target relative to the camera.
   * @param fieldToTarget The position of the target in the field.
   * @param cameraToRobot The position of the robot relative to the camera. If the camera was
   *     mounted 3 inches behind the "origin" (usually physical center) of the robot, this would be
   *     Transform2d(3 inches, 0 inches, 0 degrees).
   * @return The position of the robot in the field.
   */
  public static Pose2d estimateFieldToRobot(
      Transform2d cameraToTarget, Pose2d fieldToTarget, Transform2d cameraToRobot) {
    return estimateFieldToCamera(cameraToTarget, fieldToTarget).transformBy(cameraToRobot);
  }

  /**
   * Estimates a {@link Transform2d} that maps the camera position to the target position, using the
   * robot's gyro. Note that the gyro angle provided *must* line up with the field coordinate system
   * -- that is, it should read zero degrees when pointed towards the opposing alliance station, and
   * increase as the robot rotates CCW.
   *
   * @param cameraToTargetTranslation A Translation2d that encodes the x/y position of the target
   *     relative to the camera.
   * @param fieldToTarget A Pose2d representing the target position in the field coordinate system.
   * @param gyroAngle The current robot gyro angle, likely from odometry.
   * @return A Transform2d that takes us from the camera to the target.
   */
  public static Transform2d estimateCameraToTarget(
      Translation2d cameraToTargetTranslation, Pose2d fieldToTarget, Rotation2d gyroAngle) {
    // Map our camera at the origin out to our target, in the robot reference
    // frame. Gyro angle is needed because there's a circle of possible camera
    // poses for which the camera has the same yaw from camera to target.
    return new Transform2d(
        cameraToTargetTranslation, gyroAngle.unaryMinus().minus(fieldToTarget.getRotation()));
  }

  /**
   * Estimates the pose of the camera in the field coordinate system, given the position of the
   * target relative to the camera, and the target relative to the field. This *only* tracks the
   * position of the camera, not the position of the robot itself.
   *
   * @param cameraToTarget The position of the target relative to the camera.
   * @param fieldToTarget The position of the target in the field.
   * @return The position of the camera in the field.
   */
  public static Pose2d estimateFieldToCamera(Transform2d cameraToTarget, Pose2d fieldToTarget) {
    var targetToCamera = cameraToTarget.inverse();
    return fieldToTarget.transformBy(targetToCamera);
  }
}
