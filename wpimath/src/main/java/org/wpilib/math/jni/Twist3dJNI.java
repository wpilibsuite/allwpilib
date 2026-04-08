// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.jni;

/** Twist3d JNI. */
public final class Twist3dJNI extends WPIMathJNI {
  /**
   * Obtain a Transform3d from a (constant curvature) velocity.
   *
   * <p>The double array returned is of the form [dx, dy, dz, qw, qx, qy, qz].
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

  /** Utility class. */
  private Twist3dJNI() {}
}
