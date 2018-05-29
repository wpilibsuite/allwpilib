/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.sim.mockdata;

import edu.wpi.first.hal.sim.NotifyCallback;
import edu.wpi.first.hal.JNIWrapper;

public class RoboRioDataJNI extends JNIWrapper {
  public static native int registerFPGAButtonCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelFPGAButtonCallback(int index, int uid);
  public static native boolean getFPGAButton(int index);
  public static native void setFPGAButton(int index, boolean fPGAButton);

  public static native int registerVInVoltageCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelVInVoltageCallback(int index, int uid);
  public static native double getVInVoltage(int index);
  public static native void setVInVoltage(int index, double vInVoltage);

  public static native int registerVInCurrentCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelVInCurrentCallback(int index, int uid);
  public static native double getVInCurrent(int index);
  public static native void setVInCurrent(int index, double vInCurrent);

  public static native int registerUserVoltage6VCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelUserVoltage6VCallback(int index, int uid);
  public static native double getUserVoltage6V(int index);
  public static native void setUserVoltage6V(int index, double userVoltage6V);

  public static native int registerUserCurrent6VCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelUserCurrent6VCallback(int index, int uid);
  public static native double getUserCurrent6V(int index);
  public static native void setUserCurrent6V(int index, double userCurrent6V);

  public static native int registerUserActive6VCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelUserActive6VCallback(int index, int uid);
  public static native boolean getUserActive6V(int index);
  public static native void setUserActive6V(int index, boolean userActive6V);

  public static native int registerUserVoltage5VCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelUserVoltage5VCallback(int index, int uid);
  public static native double getUserVoltage5V(int index);
  public static native void setUserVoltage5V(int index, double userVoltage5V);

  public static native int registerUserCurrent5VCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelUserCurrent5VCallback(int index, int uid);
  public static native double getUserCurrent5V(int index);
  public static native void setUserCurrent5V(int index, double userCurrent5V);

  public static native int registerUserActive5VCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelUserActive5VCallback(int index, int uid);
  public static native boolean getUserActive5V(int index);
  public static native void setUserActive5V(int index, boolean userActive5V);

  public static native int registerUserVoltage3V3Callback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelUserVoltage3V3Callback(int index, int uid);
  public static native double getUserVoltage3V3(int index);
  public static native void setUserVoltage3V3(int index, double userVoltage3V3);

  public static native int registerUserCurrent3V3Callback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelUserCurrent3V3Callback(int index, int uid);
  public static native double getUserCurrent3V3(int index);
  public static native void setUserCurrent3V3(int index, double userCurrent3V3);

  public static native int registerUserActive3V3Callback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelUserActive3V3Callback(int index, int uid);
  public static native boolean getUserActive3V3(int index);
  public static native void setUserActive3V3(int index, boolean userActive3V3);

  public static native int registerUserFaults6VCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelUserFaults6VCallback(int index, int uid);
  public static native int getUserFaults6V(int index);
  public static native void setUserFaults6V(int index, int userFaults6V);

  public static native int registerUserFaults5VCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelUserFaults5VCallback(int index, int uid);
  public static native int getUserFaults5V(int index);
  public static native void setUserFaults5V(int index, int userFaults5V);

  public static native int registerUserFaults3V3Callback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelUserFaults3V3Callback(int index, int uid);
  public static native int getUserFaults3V3(int index);
  public static native void setUserFaults3V3(int index, int userFaults3V3);

  public static native void resetData(int index);
}
