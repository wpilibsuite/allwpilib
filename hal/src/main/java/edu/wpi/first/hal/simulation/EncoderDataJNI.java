/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class EncoderDataJNI extends JNIWrapper {
  public static native String getDisplayName(int index);
  public static native void setDisplayName(int index, String displayName);

  public static native int registerInitializedCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelInitializedCallback(int index, int uid);
  public static native boolean getInitialized(int index);
  public static native void setInitialized(int index, boolean initialized);

  public static native int registerCountCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelCountCallback(int index, int uid);
  public static native int getCount(int index);
  public static native void setCount(int index, int count);

  public static native int registerPeriodCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelPeriodCallback(int index, int uid);
  public static native double getPeriod(int index);
  public static native void setPeriod(int index, double period);

  public static native int registerResetCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelResetCallback(int index, int uid);
  public static native boolean getReset(int index);
  public static native void setReset(int index, boolean reset);

  public static native int registerMaxPeriodCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelMaxPeriodCallback(int index, int uid);
  public static native double getMaxPeriod(int index);
  public static native void setMaxPeriod(int index, double maxPeriod);

  public static native int registerDirectionCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelDirectionCallback(int index, int uid);
  public static native boolean getDirection(int index);
  public static native void setDirection(int index, boolean direction);

  public static native int registerReverseDirectionCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelReverseDirectionCallback(int index, int uid);
  public static native boolean getReverseDirection(int index);
  public static native void setReverseDirection(int index, boolean reverseDirection);

  public static native int registerSamplesToAverageCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelSamplesToAverageCallback(int index, int uid);
  public static native int getSamplesToAverage(int index);
  public static native void setSamplesToAverage(int index, int samplesToAverage);

  public static native void setDistance(int index, double distance);
  public static native double getDistance(int index);
  public static native void setRate(int index, double rate);
  public static native double getRate(int index);

  public static native void resetData(int index);

  public static native int findForChannel(int channel);
}
