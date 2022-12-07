// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.HALValue;
import edu.wpi.first.hal.JNIWrapper;

public class SimDeviceDataJNI extends JNIWrapper {
  public static native void setSimDeviceEnabled(String prefix, boolean enabled);

  public static native boolean isSimDeviceEnabled(String name);

  public static native int registerSimDeviceCreatedCallback(
      String prefix, SimDeviceCallback callback, boolean initialNotify);

  public static native void cancelSimDeviceCreatedCallback(int uid);

  public static native int registerSimDeviceFreedCallback(
      String prefix, SimDeviceCallback callback, boolean initialNotify);

  public static native void cancelSimDeviceFreedCallback(int uid);

  public static native int getSimDeviceHandle(String name);

  public static native String getSimDeviceName(int handle);

  public static native int getSimValueDeviceHandle(int handle);

  @SuppressWarnings("MemberName")
  public static class SimDeviceInfo {
    public String name;
    public int handle;

    /**
     * SimDeviceInfo constructor.
     *
     * @param name SimDevice name.
     * @param handle SimDevice handle.
     */
    public SimDeviceInfo(String name, int handle) {
      this.name = name;
      this.handle = handle;
    }
  }

  public static native SimDeviceInfo[] enumerateSimDevices(String prefix);

  public static native int registerSimValueCreatedCallback(
      int device, SimValueCallback callback, boolean initialNotify);

  public static native void cancelSimValueCreatedCallback(int uid);

  public static native int registerSimValueChangedCallback(
      int handle, SimValueCallback callback, boolean initialNotify);

  public static native void cancelSimValueChangedCallback(int uid);

  /**
   * Register a callback for SimDeviceJNI.resetSimValue(). The callback is called with the old
   * value.
   *
   * @param handle simulated value handle
   * @param callback callback
   * @param initialNotify ignored (present for consistency)
   * @return TODO
   */
  public static native int registerSimValueResetCallback(
      int handle, SimValueCallback callback, boolean initialNotify);

  public static native void cancelSimValueResetCallback(int uid);

  public static native int getSimValueHandle(int device, String name);

  @SuppressWarnings("MemberName")
  public static class SimValueInfo {
    public String name;
    public int handle;
    public int direction;
    public HALValue value;

    /**
     * SimValueInfo constructor.
     *
     * @param name SimValue name.
     * @param handle SimValue handle.
     * @param direction SimValue direction.
     * @param type SimValue type.
     * @param value1 Value 1.
     * @param value2 Value 2.
     */
    public SimValueInfo(
        String name, int handle, int direction, int type, long value1, double value2) {
      this.name = name;
      this.handle = handle;
      this.direction = direction;
      this.value = HALValue.fromNative(type, value1, value2);
    }
  }

  public static native SimValueInfo[] enumerateSimValues(int device);

  public static native String[] getSimValueEnumOptions(int handle);

  public static native double[] getSimValueEnumDoubleValues(int handle);

  public static native void resetSimDeviceData();
}
