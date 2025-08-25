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
   * @param twistDx The twist's dx value.
   * @param twistDy The twist's dy value.
   * @param twistDz The twist's dz value.
   * @param twistRx The twist's rx value.
   * @param twistRy The twist's ry value.
   * @param twistRz The twist's rz value.
   * @return The new pose as a double array.
   */
  public static native double[] exp(
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
   * @param relX The transform's translational X component.
   * @param relY The transform's translational Y component.
   * @param relZ The transform's translational Z component.
   * @param relQw The transform quaternion's W component.
   * @param relQx The transform quaternion's X component.
   * @param relQy The transform quaternion's Y component.
   * @param relQz The transform quaternion's Z component.
   * @return The twist that maps start to end as a double array.
   */
  public static native double[] log(
      double relX,
      double relY,
      double relZ,
      double relQw,
      double relQx,
      double relQy,
      double relQz);

  /** Utility class. */
  private Pose3dJNI() {}
}
