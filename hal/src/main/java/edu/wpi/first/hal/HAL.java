// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import java.util.ArrayList;
import java.util.List;

/**
 * JNI Wrapper for Hardware Abstraction Layer (HAL).
 *
 * @see "hal/HALBase.h"
 * @see "hal/Main.h"
 * @see "hal/FRCUsageReporting.h"
 */
public final class HAL extends JNIWrapper {
  /**
   * Call this to start up HAL. This is required for robot programs.
   *
   * <p>This must be called before any other HAL functions. Failure to do so will result in
   * undefined behavior, and likely segmentation faults. This means that any statically initialized
   * variables in a program MUST call this function in their constructors if they want to use other
   * HAL calls.
   *
   * <p>The common parameters are 500 for timeout and 0 for mode.
   *
   * <p>This function is safe to call from any thread, and as many times as you wish. It internally
   * guards from any reentrancy.
   *
   * <p>The applicable modes are: 0: Try to kill an existing HAL from another program, if not
   * successful, error. 1: Force kill a HAL from another program. 2: Just warn if another hal exists
   * and cannot be killed. Will likely result in undefined behavior.
   *
   * @param timeout the initialization timeout (ms)
   * @param mode the initialization mode (see remarks)
   * @return true if initialization was successful, otherwise false.
   * @see "HAL_Initialize"
   */
  public static native boolean initialize(int timeout, int mode);

  /**
   * Call this to shut down HAL.
   *
   * <p>This must be called at termination of the robot program to avoid potential segmentation
   * faults with simulation extensions at exit.
   *
   * @see "HAL_Shutdown"
   */
  public static native void shutdown();

  /**
   * Returns true if HAL_SetMain() has been called.
   *
   * @return True if HAL_SetMain() has been called, false otherwise.
   * @see "HAL_HasMain"
   */
  public static native boolean hasMain();

  /**
   * Runs the main function provided to HAL_SetMain().
   *
   * <p>If HAL_SetMain() has not been called, simply sleeps until exitMain() is called.
   *
   * @see "HAL_RunMain"
   */
  public static native void runMain();

  /**
   * Causes HAL_RunMain() to exit.
   *
   * <p>If HAL_SetMain() has been called, this calls the exit function provided to that function.
   *
   * @see "HAL_ExitMain"
   */
  public static native void exitMain();

  /** Terminates the executable (at the native level). Does nothing in simulation. */
  public static native void terminate();

  private static native void simPeriodicBeforeNative();

  private static final List<Runnable> s_simPeriodicBefore = new ArrayList<>();

  /** A callback to be run by IterativeRobotBase prior to the user's simulationPeriodic code. */
  public static final class SimPeriodicBeforeCallback implements AutoCloseable {
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

  /** A callback to be run by IterativeRobotBase after the user's simulationPeriodic code. */
  public static final class SimPeriodicAfterCallback implements AutoCloseable {
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

  /**
   * Gets if the system is in a browned out state.
   *
   * @return true if the system is in a low voltage brown out, false otherwise
   * @see "HAL_GetBrownedOut"
   */
  public static native boolean getBrownedOut();

  /**
   * Gets if the system outputs are currently active.
   *
   * @return true if the system outputs are active, false if disabled
   * @see "HAL_GetSystemActive"
   */
  public static native boolean getSystemActive();

  /**
   * Gets the current state of the Robot Signal Light (RSL).
   *
   * @return The current state of the RSL- true if on, false if off
   * @see "HAL_GetRSLState"
   */
  public static native boolean getRSLState();

  /**
   * Gets the number of times the system has been disabled due to communication errors with the
   * Driver Station.
   *
   * @return number of disables due to communication errors.
   */
  public static native int getCommsDisableCount();

  /**
   * Gets if the system time is valid.
   *
   * @return True if the system time is valid, false otherwise
   * @see "HAL_GetSystemTimeValid"
   */
  public static native boolean getSystemTimeValid();

  /**
   * Gets a port handle for a specific channel and module.
   *
   * <p>This is expected to be used for PCMs, as the roboRIO does not work with modules anymore.
   *
   * <p>The created handle does not need to be freed.
   *
   * @param module the module number
   * @param channel the channel number
   * @return the created port
   * @see "HAL_GetPortWithModule"
   */
  public static native int getPortWithModule(byte module, byte channel);

  /**
   * Gets a port handle for a specific channel.
   *
   * <p>The created handle does not need to be freed.
   *
   * @param channel the channel number
   * @return the created port
   * @see "HAL_GetPort"
   */
  public static native int getPort(byte channel);

  /**
   * Report the usage of a resource of interest.
   *
   * <p>Original signature: <code>uint32_t report(tResourceType, uint8_t, uint8_t, const
   * char*)</code>
   *
   * @param resource one of the values in the tResourceType above.
   * @param instanceNumber an index that identifies the resource instance.
   * @see "HAL_Report"
   */
  public static void report(int resource, int instanceNumber) {
    report(resource, instanceNumber, 0, "");
  }

  /**
   * Report the usage of a resource of interest.
   *
   * <p>Original signature: <code>uint32_t report(tResourceType, uint8_t, uint8_t, const
   * char*)</code>
   *
   * @param resource one of the values in the tResourceType above.
   * @param instanceNumber an index that identifies the resource instance.
   * @param context an optional additional context number for some cases (such as module number).
   *     Set to 0 to omit.
   * @see "HAL_Report"
   */
  public static void report(int resource, int instanceNumber, int context) {
    report(resource, instanceNumber, context, "");
  }

  /**
   * Report the usage of a resource of interest.
   *
   * <p>Original signature: <code>uint32_t report(tResourceType, uint8_t, uint8_t, const
   * char*)</code>
   *
   * @param resource one of the values in the tResourceType above.
   * @param instanceNumber an index that identifies the resource instance.
   * @param context an optional additional context number for some cases (such as module number).
   *     Set to 0 to omit.
   * @param feature a string to be included describing features in use on a specific resource.
   *     Setting the same resource more than once allows you to change the feature string.
   * @return the index of the added value in NetComm
   * @see "HAL_Report"
   */
  public static int report(int resource, int instanceNumber, int context, String feature) {
    return DriverStationJNI.report(resource, instanceNumber, context, feature);
  }

  private HAL() {}
}
