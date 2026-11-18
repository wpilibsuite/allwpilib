// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import org.wpilib.math.geometry.Transform3d;
import org.wpilib.math.geometry.Twist3d;

public final class Geometry3dBenchmark {
  // Non-final so the JIT can't constant-fold the inputs
  private static Twist3d twist = new Twist3d(1.0, 2.0, 0.5, 0.3, -0.2, 0.7);
  private static Transform3d transform = twist.exp();

  private Geometry3dBenchmark() {
    // Utility class.
  }

  /**
   * Twist3d.exp() benchmark.
   *
   * @return Transform3d from the twist.
   */
  public static Transform3d twist3dExp() {
    return twist.exp();
  }

  /**
   * Transform3d.log() benchmark.
   *
   * @return Twist3d from the transform.
   */
  public static Twist3d transform3dLog() {
    return transform.log();
  }
}
