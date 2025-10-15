// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.jni;

/** Transform3d JNI. */
public final class Transform3dJNI extends WPIMathJNI {
  /**
   * Returns a Twist3d that maps the Transform3d (pose delta).
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
  private Transform3dJNI() {}
}
