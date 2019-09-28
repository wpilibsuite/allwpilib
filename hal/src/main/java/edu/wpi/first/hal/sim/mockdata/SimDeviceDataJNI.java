/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.sim.mockdata;

import edu.wpi.first.hal.sim.SimDeviceCallback;
import edu.wpi.first.hal.sim.SimValueCallback;
import edu.wpi.first.hal.HALValue;
import edu.wpi.first.hal.JNIWrapper;

public class SimDeviceDataJNI extends JNIWrapper {
  public static native int registerSimDeviceCreatedCallback(String prefix, SimDeviceCallback callback, boolean initialNotify);
  public static native void cancelSimDeviceCreatedCallback(int uid);

  public static native int registerSimDeviceFreedCallback(String prefix, SimDeviceCallback callback);
  public static native void cancelSimDeviceFreedCallback(int uid);

  public static native int getSimDeviceHandle(String name);

  public static native int getSimValueDeviceHandle(int handle);

  public static class SimDeviceInfo {
    public SimDeviceInfo(String name, int handle) {
      this.name = name;
      this.handle = handle;
    }

    @SuppressWarnings("MemberName")
    String name;

    @SuppressWarnings("MemberName")
    int handle;
  }
  public static native SimDeviceInfo[] enumerateSimDevices(String prefix);

  public static native int registerSimValueCreatedCallback(int device, SimValueCallback callback, boolean initialNotify);
  public static native void cancelSimValueCreatedCallback(int uid);

  public static native int registerSimValueChangedCallback(int handle, SimValueCallback callback, boolean initialNotify);
  public static native void cancelSimValueChangedCallback(int uid);

  public static native int getSimValueHandle(int device, String name);

  public static class SimValueInfo {
    public SimValueInfo(String name, int handle, boolean readonly, int type, long value1, double value2) {
      this.name = name;
      this.handle = handle;
      this.readonly = readonly;
      this.value = HALValue.fromNative(type, value1, value2);
    }

    @SuppressWarnings("MemberName")
    String name;

    @SuppressWarnings("MemberName")
    int handle;

    @SuppressWarnings("MemberName")
    boolean readonly;

    @SuppressWarnings("MemberName")
    HALValue value;
  }
  public static native SimValueInfo[] enumerateSimValues(int device);

  public static native String[] getSimValueEnumOptions(int handle);

  public static native void resetSimDeviceData();
}
