// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.jni;

import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

/** ArmFeedforward JNI. */
public final class ArmFeedforwardJNI {
  static boolean libraryLoaded = false;

  static {
    if (Helper.getExtractOnStaticLoad()) {
      try {
        RuntimeLoader.loadLibrary("wpimathjni");
      } catch (Exception ex) {
        ex.printStackTrace();
        System.exit(1);
      }
      libraryLoaded = true;
    }
  }

  /**
   * Force load the library.
   *
   * @throws IOException If the library could not be loaded or found.
   */
  public static synchronized void forceLoad() throws IOException {
    if (libraryLoaded) {
      return;
    }
    RuntimeLoader.loadLibrary("wpimathjni");
    libraryLoaded = true;
  }

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

  /** Sets whether JNI should be loaded in the static block. */
  public static class Helper {
    private static AtomicBoolean extractOnStaticLoad = new AtomicBoolean(true);

    /**
     * Returns true if the JNI should be loaded in the static block.
     *
     * @return True if the JNI should be loaded in the static block.
     */
    public static boolean getExtractOnStaticLoad() {
      return extractOnStaticLoad.get();
    }

    /**
     * Sets whether the JNI should be loaded in the static block.
     *
     * @param load Whether the JNI should be loaded in the static block.
     */
    public static void setExtractOnStaticLoad(boolean load) {
      extractOnStaticLoad.set(load);
    }

    /** Utility class. */
    private Helper() {}
  }

  /** Utility class. */
  private ArmFeedforwardJNI() {}
}
