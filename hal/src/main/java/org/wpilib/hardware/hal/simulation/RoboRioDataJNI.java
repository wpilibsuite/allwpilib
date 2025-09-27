// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

/** JNI for roboRIO data. */
public class RoboRioDataJNI extends JNIWrapper {
  public static native int registerVInVoltageCallback(
      NotifyCallback callback, boolean initialNotify);

  public static native void cancelVInVoltageCallback(int uid);

  public static native double getVInVoltage();

  public static native void setVInVoltage(double vInVoltage);

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

  public static native void resetData();

  /** Utility class. */
  private RoboRioDataJNI() {}
}
