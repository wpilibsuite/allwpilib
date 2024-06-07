// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.jni;

import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

/** Ellipse2d JNI. */
public final class Ellipse2dJNI {
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
   * Returns the nearest point that is contained within the ellipse.
   *
   * <p>Constructs an Ellipse2d object and runs its FindNearestPoint() method.
   *
   * @param centerX The x coordinate of the center of the ellipse in meters.
   * @param centerY The y coordinate of the center of the ellipse in meters.
   * @param centerHeading The ellipse's rotation in radians.
   * @param xSemiAxis The x semi-axis in meters.
   * @param ySemiAxis The y semi-axis in meters.
   * @param pointX The x coordinate of the point that this will find the nearest point to.
   * @param pointY The y coordinate of the point that this will find the nearest point to.
   * @param nearestPoint Array to store nearest point into.
   */
  public static native void findNearestPoint(
      double centerX,
      double centerY,
      double centerHeading,
      double xSemiAxis,
      double ySemiAxis,
      double pointX,
      double pointY,
      double[] nearestPoint);

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
  private Ellipse2dJNI() {}
}
