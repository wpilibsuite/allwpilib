// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

/** JNI for roboRIO data. */
public class RoboRioDataJNI extends JNIWrapper {
  public static native int registerFPGAButtonCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelFPGAButtonCallback(int uid);

  public static native boolean getFPGAButton();

  public static native void setFPGAButton(boolean fPGAButton);

  public static native int registerVInVoltageCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelVInVoltageCallback(int uid);

  public static native double getVInVoltage();

  public static native void setVInVoltage(double vInVoltage);

  public static native int registerVInCurrentCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelVInCurrentCallback(int uid);

  public static native double getVInCurrent();

  public static native void setVInCurrent(double vInCurrent);

  public static native int registerUserVoltage6VCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelUserVoltage6VCallback(int uid);

  public static native double getUserVoltage6V();

  public static native void setUserVoltage6V(double userVoltage6V);

  public static native int registerUserCurrent6VCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelUserCurrent6VCallback(int uid);

  public static native double getUserCurrent6V();

  public static native void setUserCurrent6V(double userCurrent6V);

  public static native int registerUserActive6VCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelUserActive6VCallback(int uid);

  public static native boolean getUserActive6V();

  public static native void setUserActive6V(boolean userActive6V);

  public static native int registerUserVoltage5VCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelUserVoltage5VCallback(int uid);

  public static native double getUserVoltage5V();

  public static native void setUserVoltage5V(double userVoltage5V);

  public static native int registerUserCurrent5VCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelUserCurrent5VCallback(int uid);

  public static native double getUserCurrent5V();

  public static native void setUserCurrent5V(double userCurrent5V);

  public static native int registerUserActive5VCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelUserActive5VCallback(int uid);

  public static native boolean getUserActive5V();

  public static native void setUserActive5V(boolean userActive5V);

  public static native int registerUserVoltage3V3Callback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelUserVoltage3V3Callback(int uid);

  public static native double getUserVoltage3V3();

  public static native void setUserVoltage3V3(double userVoltage3V3);

  public static native int registerUserCurrent3V3Callback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelUserCurrent3V3Callback(int uid);

  public static native double getUserCurrent3V3();

  public static native void setUserCurrent3V3(double userCurrent3V3);

  public static native int registerUserActive3V3Callback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelUserActive3V3Callback(int uid);

  public static native boolean getUserActive3V3();

  public static native void setUserActive3V3(boolean userActive3V3);

  public static native int registerUserFaults6VCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelUserFaults6VCallback(int uid);

  public static native int getUserFaults6V();

  public static native void setUserFaults6V(int userFaults6V);

  public static native int registerUserFaults5VCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelUserFaults5VCallback(int uid);

  public static native int getUserFaults5V();

  public static native void setUserFaults5V(int userFaults5V);

  public static native int registerUserFaults3V3Callback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelUserFaults3V3Callback(int uid);

  public static native int getUserFaults3V3();

  public static native void setUserFaults3V3(int userFaults3V3);

  public static native int registerBrownoutVoltageCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelBrownoutVoltageCallback(int uid);

  public static native double getBrownoutVoltage();

  public static native void setBrownoutVoltage(double brownoutVoltage);

  public static native int registerCPUTempCallback(NotifyCallback callback, boolean initialNotify);

  public static native void cancelCPUTempCallback(int uid);

  public static native double getCPUTemp();

  public static native void setCPUTemp(double cpuTemp);

  public static native int registerTeamNumberCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelTeamNumberCallback(int uid);

  public static native int getTeamNumber();

  public static native void setTeamNumber(int teamNumber);

  public static native String getSerialNumber();

  public static native void setSerialNumber(String serialNumber);

  public static native String getComments();

  public static native void setComments(String comments);

  public static native int registerRadioLEDStateCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelRadioLEDStateCallback(int uid);

  public static native int getRadioLEDState();

  public static native void setRadioLEDState(int state);

  public static native void resetData();

  /** Utility class. */
  private RoboRioDataJNI() {}
}
