/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class PCMDataJNI extends JNIWrapper {
  public static native int registerSolenoidInitializedCallback(int index, int channel, NotifyCallback callback, boolean initialNotify);
  public static native void cancelSolenoidInitializedCallback(int index, int channel, int uid);
  public static native boolean getSolenoidInitialized(int index, int channel);
  public static native void setSolenoidInitialized(int index, int channel, boolean solenoidInitialized);

  public static native String getSolenoidDisplayName(int index, int channel);
  public static native void setSolenoidDisplayName(int index, int channel, String displayName);

  public static native int registerSolenoidOutputCallback(int index, int channel, NotifyCallback callback, boolean initialNotify);
  public static native void cancelSolenoidOutputCallback(int index, int channel, int uid);
  public static native boolean getSolenoidOutput(int index, int channel);
  public static native void setSolenoidOutput(int index, int channel, boolean solenoidOutput);

  public static native int registerCompressorInitializedCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelCompressorInitializedCallback(int index, int uid);
  public static native boolean getCompressorInitialized(int index);
  public static native void setCompressorInitialized(int index, boolean compressorInitialized);

  public static native int registerCompressorOnCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelCompressorOnCallback(int index, int uid);
  public static native boolean getCompressorOn(int index);
  public static native void setCompressorOn(int index, boolean compressorOn);

  public static native int registerClosedLoopEnabledCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelClosedLoopEnabledCallback(int index, int uid);
  public static native boolean getClosedLoopEnabled(int index);
  public static native void setClosedLoopEnabled(int index, boolean closeLoopEnabled);

  public static native int registerPressureSwitchCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelPressureSwitchCallback(int index, int uid);
  public static native boolean getPressureSwitch(int index);
  public static native void setPressureSwitch(int index, boolean pressureSwitch);

  public static native int registerCompressorCurrentCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelCompressorCurrentCallback(int index, int uid);
  public static native double getCompressorCurrent(int index);
  public static native void setCompressorCurrent(int index, double compressorCurrent);

  public static native void registerAllNonSolenoidCallbacks(int index, NotifyCallback callback, boolean initialNotify);
  public static native void registerAllSolenoidCallbacks(int index, int channel, NotifyCallback callback, boolean initialNotify);

  public static native void resetData(int index);
}
