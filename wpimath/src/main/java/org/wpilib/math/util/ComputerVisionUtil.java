// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Transform3d;

/** Computer vision utility functions. */
public final class ComputerVisionUtil {
  private ComputerVisionUtil() {
    throw new AssertionError("utility class");
  }

  /**
   * Returns the robot's pose in the field coordinate system given an object's field-relative pose,
   * the transformation from the camera's pose to the object's pose (obtained via computer vision),
   * and the transformation from the robot's pose to the camera's pose.
   *
   * <p>The object could be a target or a fiducial marker.
   *
   * @param objectInField An object's field-relative pose.
   * @param cameraToObject The transformation from the camera's pose to the object's pose. This
   *     comes from computer vision.
   * @param robotToCamera The transformation from the robot's pose to the camera's pose. This can
   *     either be a constant for a rigidly mounted camera, or variable if the camera is mounted to
   *     a turret. If the camera was mounted 3 inches in front of the "origin" (usually physical
   *     center) of the robot, this would be new Transform3d(Units.inchesToMeters(3.0), 0.0, 0.0,
   *     Rotation3d.kZero).
   * @return The robot's field-relative pose.
   */
  public static Pose3d objectToRobotPose(
      Pose3d objectInField, Transform3d cameraToObject, Transform3d robotToCamera) {
    final var objectToCamera = cameraToObject.inverse();
    final var cameraToRobot = robotToCamera.inverse();
    return objectInField.plus(objectToCamera).plus(cameraToRobot);
  }
}
