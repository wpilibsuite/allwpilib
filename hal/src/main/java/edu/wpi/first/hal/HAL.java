/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * JNI Wrapper for HAL<br>.
 */
@SuppressWarnings({"AbbreviationAsWordInName", "MethodName"})
public final class HAL extends JNIWrapper {
  public static native void waitForDSData();

  public static native boolean initialize(int timeout, int mode);

  public static native void shutdown();

  public static native boolean hasMain();

  public static native void runMain();

  public static native void exitMain();

  private static native void simPeriodicBeforeNative();

  public static final List<Runnable> s_simPeriodicBefore = new ArrayList<>();

  /**
   * Runs SimPeriodicBefore callbacks.  IterativeRobotBase calls this prior
   * to the user's simulationPeriodic code.
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

  public static final List<Runnable> s_simPeriodicAfter = new ArrayList<>();

  /**
   * Runs SimPeriodicAfter callbacks.  IterativeRobotBase calls this after
   * the user's simulationPeriodic code.
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
   * Report the usage of a resource of interest. <br>
   *
   * <p>Original signature: <code>uint32_t report(tResourceType, uint8_t, uint8_t, const
   * char*)</code>
   *
   * @param resource       one of the values in the tResourceType above (max value 51). <br>
   * @param instanceNumber an index that identifies the resource instance. <br>
   * @param context        an optional additional context number for some cases (such as module
   *                       number). Set to 0 to omit. <br>
   * @param feature        a string to be included describing features in use on a specific
   *                       resource. Setting the same resource more than once allows you to change
   *                       the feature string.
   */
  public static native int report(int resource, int instanceNumber, int context, String feature);

  public static native int nativeGetControlWord();

  @SuppressWarnings("JavadocMethod")
  public static void getControlWord(ControlWord controlWord) {
    int word = nativeGetControlWord();
    controlWord.update((word & 1) != 0, ((word >> 1) & 1) != 0, ((word >> 2) & 1) != 0,
        ((word >> 3) & 1) != 0, ((word >> 4) & 1) != 0, ((word >> 5) & 1) != 0);
  }

  private static native int nativeGetAllianceStation();

  @SuppressWarnings("JavadocMethod")
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

  @SuppressWarnings("JavadocMethod")
  public static native boolean isNewControlData();

  @SuppressWarnings("JavadocMethod")
  public static native void releaseDSMutex();

  @SuppressWarnings("JavadocMethod")
  public static native boolean waitForDSDataTimeout(double timeout);

  public static int kMaxJoystickAxes = 12;
  public static int kMaxJoystickPOVs = 12;

  public static native short getJoystickAxes(byte joystickNum, float[] axesArray);

  public static native short getJoystickPOVs(byte joystickNum, short[] povsArray);

  public static native int getJoystickButtons(byte joystickNum, ByteBuffer count);

  public static native int setJoystickOutputs(byte joystickNum, int outputs, short leftRumble,
                                              short rightRumble);

  public static native int getJoystickIsXbox(byte joystickNum);

  public static native int getJoystickType(byte joystickNum);

  public static native String getJoystickName(byte joystickNum);

  public static native int getJoystickAxisType(byte joystickNum, byte axis);

  public static native double getMatchTime();

  public static native boolean getSystemActive();

  public static native boolean getBrownedOut();

  public static native int getMatchInfo(MatchInfoData info);

  public static native int sendError(boolean isError, int errorCode, boolean isLVCode,
                                     String details, String location, String callStack,
                                     boolean printMsg);

  public static native int sendConsoleLine(String line);

  public static native int getPortWithModule(byte module, byte channel);

  public static native int getPort(byte channel);

  private HAL() {

  }
}
