// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class DriverStationDataJNI extends JNIWrapper {
  public static native int registerEnabledCallback(NotifyCallback callback, boolean initialNotify);

  public static native void cancelEnabledCallback(int uid);

  public static native boolean getEnabled();

  public static native void setEnabled(boolean enabled);

  public static native int registerAutonomousCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelAutonomousCallback(int uid);

  public static native boolean getAutonomous();

  public static native void setAutonomous(boolean autonomous);

  public static native int registerTestCallback(NotifyCallback callback, boolean initialNotify);

  public static native void cancelTestCallback(int uid);

  public static native boolean getTest();

  public static native void setTest(boolean test);

  public static native int registerEStopCallback(NotifyCallback callback, boolean initialNotify);

  public static native void cancelEStopCallback(int uid);

  public static native boolean getEStop();

  public static native void setEStop(boolean eStop);

  public static native int registerFmsAttachedCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelFmsAttachedCallback(int uid);

  public static native boolean getFmsAttached();

  public static native void setFmsAttached(boolean fmsAttached);

  public static native int registerDsAttachedCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelDsAttachedCallback(int uid);

  public static native boolean getDsAttached();

  public static native void setDsAttached(boolean dsAttached);

  public static native int registerAllianceStationIdCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelAllianceStationIdCallback(int uid);

  public static native int getAllianceStationId();

  public static native void setAllianceStationId(int allianceStationId);

  public static native int registerMatchTimeCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelMatchTimeCallback(int uid);

  public static native double getMatchTime();

  public static native void setMatchTime(double matchTime);

  public static native void setJoystickAxes(byte joystickNum, float[] axesArray);

  public static native void setJoystickPOVs(byte joystickNum, short[] povsArray);

  public static native void setJoystickButtons(byte joystickNum, int buttons, int count);

  public static native long getJoystickOutputs(int stick);

  public static native int getJoystickRumble(int stick, int rumbleNum);

  public static native void setMatchInfo(
      String eventName,
      String gameSpecificMessage,
      int matchNumber,
      int replayNumber,
      int matchType);

  public static native void registerAllCallbacks(NotifyCallback callback, boolean initialNotify);

  public static native void notifyNewData();

  public static native void setSendError(boolean shouldSend);

  public static native void setSendConsoleLine(boolean shouldSend);

  public static native void setJoystickButton(int stick, int button, boolean state);

  public static native void setJoystickAxis(int stick, int axis, double value);

  public static native void setJoystickPOV(int stick, int pov, int value);

  public static native void setJoystickButtonsValue(int stick, int buttons);

  public static native void setJoystickAxisCount(int stick, int count);

  public static native void setJoystickPOVCount(int stick, int count);

  public static native void setJoystickButtonCount(int stick, int count);

  public static native void setJoystickIsXbox(int stick, boolean isXbox);

  public static native void setJoystickType(int stick, int type);

  public static native void setJoystickName(int stick, String name);

  public static native void setJoystickAxisType(int stick, int axis, int type);

  public static native void setGameSpecificMessage(String message);

  public static native void setEventName(String name);

  public static native void setMatchType(int type);

  public static native void setMatchNumber(int matchNumber);

  public static native void setReplayNumber(int replayNumber);

  public static native void resetData();
}
