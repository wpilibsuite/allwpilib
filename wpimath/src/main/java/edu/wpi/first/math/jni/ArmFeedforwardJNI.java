// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.jni;

/** ArmFeedforward JNI. */
public final class ArmFeedforwardJNI extends WPIMathJNI {
  /**
   * Obtain a feedforward voltage from a single jointed arm feedforward object.
   *
   * <p>Constructs an ArmFeedforward object and runs its currentVelocity and nextVelocity overload
   *
   * @param ks The ArmFeedforward's static gain in volts.
   * @param kv The ArmFeedforward's velocity gain in volt seconds per radian.
   * @param ka The ArmFeedforward's acceleration gain in volt seconds² per radian.
   * @param kg The ArmFeedforward's gravity gain in volts.
   * @param currentAngle The current angle in the calculation in radians.
   * @param currentVelocity The current velocity in the calculation in radians per second.
   * @param nextVelocity The next velocity in the calculation in radians per second.
   * @param dt The time between velocity setpoints in seconds.
   * @return The calculated feedforward in volts.
   */
  public static native double calculate(
      double ks,
      double kv,
      double ka,
      double kg,
      double currentAngle,
      double currentVelocity,
      double nextVelocity,
      double dt);

  /** Utility class. */
  private ArmFeedforwardJNI() {}
}
