// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import java.util.ArrayList;
import java.util.List;

/**
 * JNI Wrapper for HAL<br>
 * .
 */
public final class HAL extends JNIWrapper {
  public static native boolean initialize(int timeout, int mode);

  public static native void shutdown();

  public static native boolean hasMain();

  public static native void runMain();

  public static native void exitMain();

  private static native void simPeriodicBeforeNative();

  private static final List<Runnable> s_simPeriodicBefore = new ArrayList<>();

  public static class SimPeriodicBeforeCallback implements AutoCloseable {
    private SimPeriodicBeforeCallback(Runnable r) {
      m_run = r;
    }

    @Override
    public void close() {
      synchronized (s_simPeriodicBefore) {
        s_simPeriodicBefore.remove(m_run);
      }
    }

    private final Runnable m_run;
  }

  /**
   * Registers a callback to be run by IterativeRobotBase prior to the user's simulationPeriodic
   * code.
   *
   * @param r runnable
   * @return Callback object (must be retained for callback to stay active).
   */
  public static SimPeriodicBeforeCallback registerSimPeriodicBeforeCallback(Runnable r) {
    synchronized (s_simPeriodicBefore) {
      s_simPeriodicBefore.add(r);
    }
    return new SimPeriodicBeforeCallback(r);
  }

  /**
   * Runs SimPeriodicBefore callbacks. IterativeRobotBase calls this prior to the user's
   * simulationPeriodic code.
   */
  public static void simPeriodicBefore() {
    simPeriodicBeforeNative();
    synchronized (s_simPeriodicBefore) {
      for (Runnable r : s_simPeriodicBefore) {
        r.run();
      }
    }
  }

  private static native void simPeriodicAfterNative();

  private static final List<Runnable> s_simPeriodicAfter = new ArrayList<>();

  public static class SimPeriodicAfterCallback implements AutoCloseable {
    private SimPeriodicAfterCallback(Runnable r) {
      m_run = r;
    }

    @Override
    public void close() {
      synchronized (s_simPeriodicAfter) {
        s_simPeriodicAfter.remove(m_run);
      }
    }

    private final Runnable m_run;
  }

  /**
   * Registers a callback to be run by IterativeRobotBase after the user's simulationPeriodic code.
   *
   * @param r runnable
   * @return Callback object (must be retained for callback to stay active).
   */
  public static SimPeriodicAfterCallback registerSimPeriodicAfterCallback(Runnable r) {
    synchronized (s_simPeriodicAfter) {
      s_simPeriodicAfter.add(r);
    }
    return new SimPeriodicAfterCallback(r);
  }

  /**
   * Runs SimPeriodicAfter callbacks. IterativeRobotBase calls this after the user's
   * simulationPeriodic code.
   */
  public static void simPeriodicAfter() {
    simPeriodicAfterNative();
    synchronized (s_simPeriodicAfter) {
      for (Runnable r : s_simPeriodicAfter) {
        r.run();
      }
    }
  }

  public static native boolean getBrownedOut();

  public static native boolean getSystemActive();

  public static native int getPortWithModule(byte module, byte channel);

  public static native int getPort(byte channel);

  public static void report(int resource, int instanceNumber) {
    report(resource, instanceNumber, 0, "");
  }

  public static void report(int resource, int instanceNumber, int context) {
    report(resource, instanceNumber, context, "");
  }

  public static int report(int resource, int instanceNumber, int context, String feature) {
    return DriverStationJNI.report(resource, instanceNumber, context, feature);
  }

  private HAL() {}
}
