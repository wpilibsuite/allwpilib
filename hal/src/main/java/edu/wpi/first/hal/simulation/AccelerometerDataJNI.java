/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class AccelerometerDataJNI extends JNIWrapper {
  public static native String getDisplayName(int index);
  public static native void setDisplayName(int index, String displayName);

  public static native int registerActiveCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelActiveCallback(int index, int uid);
  public static native boolean getActive(int index);
  public static native void setActive(int index, boolean active);

  public static native int registerRangeCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelRangeCallback(int index, int uid);
  public static native int getRange(int index);
  public static native void setRange(int index, int range);

  public static native int registerXCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelXCallback(int index, int uid);
  public static native double getX(int index);
  public static native void setX(int index, double x);

  public static native int registerYCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelYCallback(int index, int uid);
  public static native double getY(int index);
  public static native void setY(int index, double y);

  public static native int registerZCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelZCallback(int index, int uid);
  public static native double getZ(int index);
  public static native void setZ(int index, double z);

  public static native void resetData(int index);
}
