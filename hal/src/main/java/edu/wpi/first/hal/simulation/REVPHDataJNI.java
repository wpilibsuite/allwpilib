// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class REVPHDataJNI extends JNIWrapper {
  public static native int registerInitializedCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelInitializedCallback(int index, int uid);

  public static native boolean getInitialized(int index);

  public static native void setInitialized(int index, boolean initialized);

  public static native int registerSolenoidOutputCallback(
      int index, int channel, NotifyCallback callback, boolean initialNotify);

  public static native void cancelSolenoidOutputCallback(int index, int channel, int uid);

  public static native boolean getSolenoidOutput(int index, int channel);

  public static native void setSolenoidOutput(int index, int channel, boolean solenoidOutput);

  public static native int registerCompressorOnCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelCompressorOnCallback(int index, int uid);

  public static native boolean getCompressorOn(int index);

  public static native void setCompressorOn(int index, boolean compressorOn);

  public static native int registerCompressorConfigTypeCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelCompressorConfigTypeCallback(int index, int uid);

  public static native int getCompressorConfigType(int index);

  public static native void setCompressorConfigType(int index, int configType);

  public static native int registerPressureSwitchCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelPressureSwitchCallback(int index, int uid);

  public static native boolean getPressureSwitch(int index);

  public static native void setPressureSwitch(int index, boolean pressureSwitch);

  public static native int registerCompressorCurrentCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelCompressorCurrentCallback(int index, int uid);

  public static native double getCompressorCurrent(int index);

  public static native void setCompressorCurrent(int index, double compressorCurrent);

  public static native void registerAllNonSolenoidCallbacks(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void registerAllSolenoidCallbacks(
      int index, int channel, NotifyCallback callback, boolean initialNotify);

  public static native void resetData(int index);
}
