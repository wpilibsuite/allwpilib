// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import java.nio.ByteBuffer;

public class DriverStationJNI extends JNIWrapper {
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

  @SuppressWarnings("MissingJavadocMethod")
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

  public static final int kRed1AllianceStation = 0;
  public static final int kRed2AllianceStation = 1;
  public static final int kRed3AllianceStation = 2;
  public static final int kBlue1AllianceStation = 3;
  public static final int kBlue2AllianceStation = 4;
  public static final int kBlue3AllianceStation = 5;

  @SuppressWarnings("MissingJavadocMethod")
  public static AllianceStationID getAllianceStation() {
    switch (nativeGetAllianceStation()) {
      case kRed1AllianceStation:
        return AllianceStationID.Red1;
      case kRed2AllianceStation:
        return AllianceStationID.Red2;
      case kRed3AllianceStation:
        return AllianceStationID.Red3;
      case kBlue1AllianceStation:
        return AllianceStationID.Blue1;
      case kBlue2AllianceStation:
        return AllianceStationID.Blue2;
      case kBlue3AllianceStation:
        return AllianceStationID.Blue3;
      default:
        return null;
    }
  }

  public static final int kMaxJoystickAxes = 12;
  public static final int kMaxJoystickPOVs = 12;
  public static final int kMaxJoysticks = 6;

  public static native int getJoystickAxes(byte joystickNum, float[] axesArray);

  public static native int getJoystickAxesRaw(byte joystickNum, int[] rawAxesArray);

  public static native int getJoystickPOVs(byte joystickNum, short[] povsArray);

  public static native int getJoystickButtons(byte joystickNum, ByteBuffer count);

  public static native void getAllJoystickData(
      float[] axesArray, byte[] rawAxesArray, short[] povsArray, long[] buttonsAndMetadata);

  public static native int setJoystickOutputs(
      byte joystickNum, int outputs, short leftRumble, short rightRumble);

  public static native int getJoystickIsXbox(byte joystickNum);

  public static native int getJoystickType(byte joystickNum);

  public static native String getJoystickName(byte joystickNum);

  public static native int getJoystickAxisType(byte joystickNum, byte axis);

  public static native double getMatchTime();

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

  public static native void refreshDSData();

  public static native void provideNewDataEventHandle(int handle);

  public static native void removeNewDataEventHandle(int handle);

  public static native boolean getOutputsActive();

  private DriverStationJNI() {}
}
