// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.jni;

import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

/** StateSpaceUtil JNI. */
public final class StateSpaceUtilJNI {
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
   * Returns true if (A, B) is a stabilizable pair.
   *
   * <p>(A, B) is stabilizable if and only if the uncontrollable eigenvalues of A, if any, have
   * absolute values less than one, where an eigenvalue is uncontrollable if rank(lambda * I - A, B)
   * &lt; n where n is the number of states.
   *
   * @param states the number of states of the system.
   * @param inputs the number of inputs to the system.
   * @param A System matrix.
   * @param B Input matrix.
   * @return If the system is stabilizable.
   */
  public static native boolean isStabilizable(int states, int inputs, double[] A, double[] B);

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
  private StateSpaceUtilJNI() {}
}
