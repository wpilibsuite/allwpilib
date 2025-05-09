// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import java.nio.ByteBuffer;

/**
 * Driver Station JNI Functions.
 *
 * @see "hal/DriverStation.h"
 */
public class DriverStationJNI extends JNIWrapper {
  /**
   * Sets the program starting flag in the DS.
   *
   * <p>This is what changes the DS to showing robot code ready.
   *
   * @see "HAL_ObserveUserProgramStarting"
   */
  public static native void observeUserProgramStarting();

  /**
   * Sets the disabled flag in the DS.
   *
   * <p>This is used for the DS to ensure the robot is properly responding to its state request.
   * Ensure this gets called about every 50ms, or the robot will be disabled by the DS.
   *
   * @see "HAL_ObserveUserProgramDisabled"
   */
  public static native void observeUserProgramDisabled();

  /**
   * Sets the autonomous enabled flag in the DS.
   *
   * <p>This is used for the DS to ensure the robot is properly responding to its state request.
   * Ensure this gets called about every 50ms, or the robot will be disabled by the DS.
   *
   * @see "HAL_ObserveUserProgramAutonomous"
   */
  public static native void observeUserProgramAutonomous();

  /**
   * Sets the teleoperated enabled flag in the DS.
   *
   * <p>This is used for the DS to ensure the robot is properly responding to its state request.
   * Ensure this gets called about every 50ms, or the robot will be disabled by the DS.
   *
   * @see "HAL_ObserveUserProgramTeleop"
   */
  public static native void observeUserProgramTeleop();

  /**
   * Sets the test mode flag in the DS.
   *
   * <p>This is used for the DS to ensure the robot is properly responding to its state request.
   * Ensure this gets called about every 50ms, or the robot will be disabled by the DS.
   *
   * @see "HAL_ObserveUserProgramTest"
   */
  public static native void observeUserProgramTest();

  /**
   * Gets the current control word of the driver station.
   *
   * <p>The control word contains the robot state.
   *
   * @return the control word
   * @see "HAL_GetControlWord"
   * @see getControlWord for a version easier to parse
   */
  public static native int nativeGetControlWord();

  /**
   * Gets the current control word of the driver station.
   *
   * <p>The control work contains the robot state.
   *
   * @param controlWord the ControlWord to update
   * @see "HAL_GetControlWord"
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

  /**
   * Gets the current alliance station ID.
   *
   * @return the alliance station ID int
   * @see "HAL_GetAllianceStation"
   */
  private static native int nativeGetAllianceStation();

  /** Unknown Alliance Station ID. */
  public static final int UNKNOWN_ALLIANCE_STATION = 0;

  /** Red Alliance Station 1 ID. */
  public static final int RED1_ALLIANCE_STATION = 1;

  /** Red Alliance Station 2 ID. */
  public static final int RED2_ALLIANCE_STATION = 2;

  /** Red Alliance Station 3 ID. */
  public static final int RED3_ALLIANCE_STATION = 3;

  /** Blue Alliance Station 1 ID. */
  public static final int BLUE1_ALLIANCE_STATION = 4;

  /** Blue Alliance Station 2 ID. */
  public static final int BLUE2_ALLIANCE_STATION = 5;

  /** Blue Alliance Station 3 ID. */
  public static final int BLUE3_ALLIANCE_STATION = 6;

  /**
   * Gets the current alliance station ID.
   *
   * @return the alliance station ID as AllianceStationID
   * @see "HAL_GetAllianceStation"
   */
  public static AllianceStationID getAllianceStation() {
    return switch (nativeGetAllianceStation()) {
      case UNKNOWN_ALLIANCE_STATION -> AllianceStationID.Unknown;
      case RED1_ALLIANCE_STATION -> AllianceStationID.Red1;
      case RED2_ALLIANCE_STATION -> AllianceStationID.Red2;
      case RED3_ALLIANCE_STATION -> AllianceStationID.Red3;
      case BLUE1_ALLIANCE_STATION -> AllianceStationID.Blue1;
      case BLUE2_ALLIANCE_STATION -> AllianceStationID.Blue2;
      case BLUE3_ALLIANCE_STATION -> AllianceStationID.Blue3;
      default -> null;
    };
  }

  /** The maximum number of axes. */
  public static final int MAX_JOYSTICK_AXES = 12;

  /** The maximum number of POVs. */
  public static final int MAX_JOYSTICK_POVS = 12;

  /** The maximum number of joysticks. */
  public static final int MAX_JOYSTICKS = 6;

  /**
   * Gets the axes of a specific joystick.
   *
   * @param joystickNum the joystick number
   * @param axesArray the axes values
   * @return number of joystick axes, or 0 for error
   * @see "HAL_GetJoystickAxes"
   */
  public static native int getJoystickAxes(byte joystickNum, float[] axesArray);

  /**
   * Gets the axes of a specific joystick.
   *
   * @param joystickNum the joystick number
   * @param rawAxesArray the raw int axes values (0-255)
   * @return number of joystick axes, or 0 for error
   * @see "HAL_GetJoystickAxes"
   */
  public static native int getJoystickAxesRaw(byte joystickNum, int[] rawAxesArray);

  /**
   * Gets the POVs of a specific joystick.
   *
   * @param joystickNum the joystick number
   * @param povsArray the POV values
   * @return number of POVs, or 0 for error
   * @see "HAL_GetJoystickPOVs"
   */
  public static native int getJoystickPOVs(byte joystickNum, short[] povsArray);

  /**
   * Gets the buttons of a specific joystick.
   *
   * @param joystickNum the joystick number
   * @param count the count of buttons
   * @return The joystick button values
   * @see "HAL_GetJoystickButtons"
   */
  public static native int getJoystickButtons(byte joystickNum, ByteBuffer count);

  /**
   * Get all joystick data.
   *
   * @param axesArray all joystick axes
   * @param rawAxesArray all joystick axes as int
   * @param povsArray all povs
   * @param buttonsAndMetadata array of long joystick axes count, long joystick povs count, long
   *     joystick buttons count, long joystick buttons values
   * @see "HAL_GetAllJoystickData"
   */
  public static native void getAllJoystickData(
      float[] axesArray, byte[] rawAxesArray, short[] povsArray, long[] buttonsAndMetadata);

  /**
   * Set joystick outputs.
   *
   * @param joystickNum the joystick number
   * @param outputs bitmask of outputs, 1 for on 0 for off
   * @param leftRumble the left rumble value (0-FFFF)
   * @param rightRumble the right rumble value (0-FFFF)
   * @return the error code, or 0 for success
   * @see "HAL_SetJoystickOutputs"
   */
  public static native int setJoystickOutputs(
      byte joystickNum, int outputs, int leftRumble, int rightRumble);

  /**
   * Gets whether a specific joystick is considered to be an XBox controller.
   *
   * @param joystickNum the joystick number
   * @return 1 if xbox, 0 otherwise
   * @see "HAL_GetJoystickIsXbox"
   */
  public static native int getJoystickIsXbox(byte joystickNum);

  /**
   * Gets the type of joystick connected.
   *
   * <p>This is device specific, and different depending on what system input type the joystick
   * uses.
   *
   * @param joystickNum the joystick number
   * @return the enumerated joystick type
   * @see "HAL_GetJoystickType"
   */
  public static native int getJoystickType(byte joystickNum);

  /**
   * Gets the name of a joystick.
   *
   * <p>The returned array must be freed with HAL_FreeJoystickName.
   *
   * @param joystickNum the joystick number
   * @return the joystick name
   * @see "HAL_GetJoystickName"
   */
  public static native String getJoystickName(byte joystickNum);

  /**
   * Gets the type of a specific joystick axis.
   *
   * <p>This is device specific, and different depending on what system input type the joystick
   * uses.
   *
   * @param joystickNum the joystick number
   * @param axis the axis number
   * @return the enumerated axis type
   * @see "HAL_GetJoystickAxisType"
   */
  public static native int getJoystickAxisType(byte joystickNum, byte axis);

  /**
   * Returns the approximate match time.
   *
   * <p>The FMS does not send an official match time to the robots, but does send an approximate
   * match time. The value will count down the time remaining in the current period (auto or
   * teleop).
   *
   * <p>Warning: This is not an official time (so it cannot be used to dispute ref calls or
   * guarantee that a function will trigger before the match ends).
   *
   * <p>The Practice Match function of the DS approximates the behavior seen on the field.
   *
   * @return time remaining in current match period (auto or teleop)
   * @see "HAL_GetMatchTime"
   */
  public static native double getMatchTime();

  /**
   * Gets info about a specific match.
   *
   * @param info the match info to populate
   * @return the error code, or 0 for success
   * @see "HAL_GetMatchInfo"
   */
  public static native int getMatchInfo(MatchInfoData info);

  /**
   * Sends an error to the driver station.
   *
   * @param isError true for error, false for warning
   * @param errorCode the error code
   * @param isLVCode true for a LV error code, false for a standard error code
   * @param details the details of the error
   * @param location the file location of the error
   * @param callStack the callstack of the error
   * @param printMsg true to print the error message to stdout as well as to the DS
   * @return the error code, or 0 for success
   * @see "HAL_SendError"
   */
  public static native int sendError(
      boolean isError,
      int errorCode,
      boolean isLVCode,
      String details,
      String location,
      String callStack,
      boolean printMsg);

  /**
   * Sends a line to the driver station console.
   *
   * @param line the line to send
   * @return the error code, or 0 for success
   */
  public static native int sendConsoleLine(String line);

  /**
   * Refresh the DS control word.
   *
   * @return true if updated
   * @see "HAL_RefreshDSData"
   */
  public static native boolean refreshDSData();

  /**
   * Adds an event handle to be signalled when new data arrives.
   *
   * @param handle the event handle to be signalled
   */
  public static native void provideNewDataEventHandle(int handle);

  /**
   * Removes the event handle from being signalled when new data arrives.
   *
   * @param handle the event handle to remove
   */
  public static native void removeNewDataEventHandle(int handle);

  /**
   * Gets if outputs are enabled by the control system.
   *
   * @return true if outputs are enabled
   */
  public static native boolean getOutputsActive();

  /** Utility class. */
  private DriverStationJNI() {}
}
