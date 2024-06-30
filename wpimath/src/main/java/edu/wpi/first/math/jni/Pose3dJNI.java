// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.jni;

/** Pose3d JNI. */
public final class Pose3dJNI extends WPIMathJNI {
  /**
   * Obtain a Pose3d from a (constant curvature) velocity.
   *
   * <p>The double array returned is of the form [dx, dy, dz, qx, qy, qz].
   *
   * @param poseX The pose's translational X component.
   * @param poseY The pose's translational Y component.
   * @param poseZ The pose's translational Z component.
   * @param poseQw The pose quaternion's W component.
   * @param poseQx The pose quaternion's X component.
   * @param poseQy The pose quaternion's Y component.
   * @param poseQz The pose quaternion's Z component.
   * @param twistDx The twist's dx value.
   * @param twistDy The twist's dy value.
   * @param twistDz The twist's dz value.
   * @param twistRx The twist's rx value.
   * @param twistRy The twist's ry value.
   * @param twistRz The twist's rz value.
   * @return The new pose as a double array.
   */
  public static native double[] exp(
      double poseX,
      double poseY,
      double poseZ,
      double poseQw,
      double poseQx,
      double poseQy,
      double poseQz,
      double twistDx,
      double twistDy,
      double twistDz,
      double twistRx,
      double twistRy,
      double twistRz);

  /**
   * Returns a Twist3d that maps the starting pose to the end pose.
   *
   * <p>The double array returned is of the form [dx, dy, dz, rx, ry, rz].
   *
   * @param startX The starting pose's translational X component.
   * @param startY The starting pose's translational Y component.
   * @param startZ The starting pose's translational Z component.
   * @param startQw The starting pose quaternion's W component.
   * @param startQx The starting pose quaternion's X component.
   * @param startQy The starting pose quaternion's Y component.
   * @param startQz The starting pose quaternion's Z component.
   * @param endX The ending pose's translational X component.
   * @param endY The ending pose's translational Y component.
   * @param endZ The ending pose's translational Z component.
   * @param endQw The ending pose quaternion's W component.
   * @param endQx The ending pose quaternion's X component.
   * @param endQy The ending pose quaternion's Y component.
   * @param endQz The ending pose quaternion's Z component.
   * @return The twist that maps start to end as a double array.
   */
  public static native double[] log(
      double startX,
      double startY,
      double startZ,
      double startQw,
      double startQx,
      double startQy,
      double startQz,
      double endX,
      double endY,
      double endZ,
      double endQw,
      double endQx,
      double endQy,
      double endQz);

  /** Utility class. */
  private Pose3dJNI() {}
}
