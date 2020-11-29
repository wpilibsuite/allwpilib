/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class AnalogGyroDataJNI extends JNIWrapper {
  public static native String getDisplayName(int index);
  public static native void setDisplayName(int index, String displayName);

  public static native int registerAngleCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelAngleCallback(int index, int uid);
  public static native double getAngle(int index);
  public static native void setAngle(int index, double angle);

  public static native int registerRateCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelRateCallback(int index, int uid);
  public static native double getRate(int index);
  public static native void setRate(int index, double rate);

  public static native int registerInitializedCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelInitializedCallback(int index, int uid);
  public static native boolean getInitialized(int index);
  public static native void setInitialized(int index, boolean initialized);

  public static native void resetData(int index);
}
