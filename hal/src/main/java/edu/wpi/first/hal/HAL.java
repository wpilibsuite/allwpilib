// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * JNI Wrapper for HAL<br>
 * .
 */
public final class HAL extends JNIWrapper {
  public static native void waitForDSData();

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

  public static native void observeUserProgramStarting();

  public static native void observeUserProgramDisabled();

  public static native void observeUserProgramAutonomous();

  public static native void observeUserProgramTeleop();

  public static native void observeUserProgramTest();

  public static void report(int resource, int instanceNumber) {
    report(resource, instanceNumber, 0, "");
  }

  public static void report(int resource, int instanceNumber, int context) {
    report(resource, instanceNumber, context, "");
  }

  /**
   * Report the usage of a resource of interest.
   *
   * <p>Original signature: <code>uint32_t report(tResourceType, uint8_t, uint8_t, const
   * char*)</code>
   *
   * @param resource one of the values in the tResourceType above (max value 51).
   * @param instanceNumber an index that identifies the resource instance.
   * @param context an optional additional context number for some cases (such as module number).
   *     Set to 0 to omit.
   * @param feature a string to be included describing features in use on a specific resource.
   *     Setting the same resource more than once allows you to change the feature string.
   * @return TODO
   */
  public static native int report(int resource, int instanceNumber, int context, String feature);

  public static native int nativeGetControlWord();

  /**
   * Get the current DriverStation control word.
   *
   * @param controlWord Storage for control word.
   */
  public static void getControlWord(ControlWord controlWord) {
    int word = nativeGetControlWord();
    controlWord.update(
        (word & 1) != 0,
        ((word >> 1) & 1) != 0,
        ((word >> 2) & 1) != 0,
        ((word >> 3) & 1) != 0,
        ((word >> 4) & 1) != 0,
        ((word >> 5) & 1) != 0);
  }

  private static native int nativeGetAllianceStation();

  /**
   * Get the alliance station.
   *
   * @return The alliance station.
   */
  public static AllianceStationID getAllianceStation() {
    switch (nativeGetAllianceStation()) {
      case 0:
        return AllianceStationID.Red1;
      case 1:
        return AllianceStationID.Red2;
      case 2:
        return AllianceStationID.Red3;
      case 3:
        return AllianceStationID.Blue1;
      case 4:
        return AllianceStationID.Blue2;
      case 5:
        return AllianceStationID.Blue3;
      default:
        return null;
    }
  }

  public static native boolean isNewControlData();

  public static native void releaseDSMutex();

  public static native boolean waitForDSDataTimeout(double timeout);

  public static final int kMaxJoystickAxes = 12;
  public static final int kMaxJoystickPOVs = 12;

  public static native short getJoystickAxes(byte joystickNum, float[] axesArray);

  public static native short getJoystickPOVs(byte joystickNum, short[] povsArray);

  public static native int getJoystickButtons(byte joystickNum, ByteBuffer count);

  public static native int setJoystickOutputs(
      byte joystickNum, int outputs, short leftRumble, short rightRumble);

  public static native int getJoystickIsXbox(byte joystickNum);

  public static native int getJoystickType(byte joystickNum);

  public static native String getJoystickName(byte joystickNum);

  public static native int getJoystickAxisType(byte joystickNum, byte axis);

  public static native double getMatchTime();

  public static native boolean getSystemActive();

  public static native boolean getBrownedOut();

  public static native int getMatchInfo(MatchInfoData info);

  public static native int sendError(
      boolean isError,
      int errorCode,
      boolean isLVCode,
      String details,
      String location,
      String callStack,
      boolean printMsg);

  public static native int sendConsoleLine(String line);

  public static native int getPortWithModule(byte module, byte channel);

  public static native int getPort(byte channel);

  private HAL() {}
}
