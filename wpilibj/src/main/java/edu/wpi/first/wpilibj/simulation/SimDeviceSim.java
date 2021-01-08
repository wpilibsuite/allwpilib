// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimBoolean;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.hal.SimEnum;
import edu.wpi.first.hal.SimInt;
import edu.wpi.first.hal.SimLong;
import edu.wpi.first.hal.SimValue;
import edu.wpi.first.hal.simulation.SimDeviceCallback;
import edu.wpi.first.hal.simulation.SimDeviceDataJNI;
import edu.wpi.first.hal.simulation.SimValueCallback;
import edu.wpi.first.hal.simulation.SimValueCallback2;

/** Class to control the simulation side of a SimDevice. */
public class SimDeviceSim {
  private final int m_handle;

  /**
   * Constructs a SimDeviceSim.
   *
   * @param name name of the SimDevice
   */
  public SimDeviceSim(String name) {
    m_handle = SimDeviceDataJNI.getSimDeviceHandle(name);
  }

  /**
   * Get the property object with the given name.
   *
   * @param name the property name
   * @return the property object
   */
  public SimValue getValue(String name) {
    int handle = SimDeviceDataJNI.getSimValueHandle(m_handle, name);
    if (handle <= 0) {
      return null;
    }
    return new SimValue(handle);
  }

  /**
   * Get the property object with the given name.
   *
   * @param name the property name
   * @return the property object
   */
  public SimInt getInt(String name) {
    int handle = SimDeviceDataJNI.getSimValueHandle(m_handle, name);
    if (handle <= 0) {
      return null;
    }
    return new SimInt(handle);
  }

  /**
   * Get the property object with the given name.
   *
   * @param name the property name
   * @return the property object
   */
  public SimLong getLong(String name) {
    int handle = SimDeviceDataJNI.getSimValueHandle(m_handle, name);
    if (handle <= 0) {
      return null;
    }
    return new SimLong(handle);
  }

  /**
   * Get the property object with the given name.
   *
   * @param name the property name
   * @return the property object
   */
  public SimDouble getDouble(String name) {
    int handle = SimDeviceDataJNI.getSimValueHandle(m_handle, name);
    if (handle <= 0) {
      return null;
    }
    return new SimDouble(handle);
  }

  /**
   * Get the property object with the given name.
   *
   * @param name the property name
   * @return the property object
   */
  public SimEnum getEnum(String name) {
    int handle = SimDeviceDataJNI.getSimValueHandle(m_handle, name);
    if (handle <= 0) {
      return null;
    }
    return new SimEnum(handle);
  }

  /**
   * Get the property object with the given name.
   *
   * @param name the property name
   * @return the property object
   */
  public SimBoolean getBoolean(String name) {
    int handle = SimDeviceDataJNI.getSimValueHandle(m_handle, name);
    if (handle <= 0) {
      return null;
    }
    return new SimBoolean(handle);
  }

  /**
   * Get all options for the given enum.
   *
   * @param val the enum
   * @return names of the different values for that enum
   */
  public static String[] getEnumOptions(SimEnum val) {
    return SimDeviceDataJNI.getSimValueEnumOptions(val.getNativeHandle());
  }

  /**
   * Get all data of this object.
   *
   * @return all data and fields of this object
   */
  public SimDeviceDataJNI.SimValueInfo[] enumerateValues() {
    return SimDeviceDataJNI.enumerateSimValues(m_handle);
  }

  /**
   * Get the native handle of this object.
   *
   * @return the handle used to refer to this object through JNI
   */
  public int getNativeHandle() {
    return m_handle;
  }

  /**
   * Register a callback to be run every time a new value is added to this device.
   *
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerValueCreatedCallback(
      SimValueCallback callback, boolean initialNotify) {
    int uid = SimDeviceDataJNI.registerSimValueCreatedCallback(m_handle, callback, initialNotify);
    return new CallbackStore(uid, SimDeviceDataJNI::cancelSimValueCreatedCallback);
  }

  /**
   * Register a callback to be run every time a value is changed on this device.
   *
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerValueChangedCallback(
      SimValue value, SimValueCallback callback, boolean initialNotify) {
    int uid =
        SimDeviceDataJNI.registerSimValueChangedCallback(
            value.getNativeHandle(), callback, initialNotify);
    return new CallbackStore(uid, SimDeviceDataJNI::cancelSimValueChangedCallback);
  }

  /**
   * Register a callback to be run every time a value is changed on this device.
   *
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerValueChangedCallback2(
      SimValue value, SimValueCallback2 callback, boolean initialNotify) {
    int uid =
        SimDeviceDataJNI.registerSimValueChangedCallback2(
            value.getNativeHandle(), callback, initialNotify);
    return new CallbackStore(uid, SimDeviceDataJNI::cancelSimValueChangedCallback);
  }

  /**
   * Register a callback for SimDouble.reset() and similar functions. The callback is called with
   * the old value.
   *
   * @param value simulated value
   * @param callback callback
   * @param initialNotify ignored (present for consistency)
   */
  public CallbackStore registerValueResetCallback(
      SimValue value, SimValueCallback2 callback, boolean initialNotify) {
    int uid =
        SimDeviceDataJNI.registerSimValueResetCallback(
            value.getNativeHandle(), callback, initialNotify);
    return new CallbackStore(uid, SimDeviceDataJNI::cancelSimValueResetCallback);
  }

  /**
   * Get all sim devices with the given prefix.
   *
   * @param prefix the prefix to filter sim devices
   * @return all sim devices
   */
  public static SimDeviceDataJNI.SimDeviceInfo[] enumerateDevices(String prefix) {
    return SimDeviceDataJNI.enumerateSimDevices(prefix);
  }

  /**
   * Register a callback to be run every time a new {@link edu.wpi.first.hal.SimDevice} is created.
   *
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public static CallbackStore registerDeviceCreatedCallback(
      String prefix, SimDeviceCallback callback, boolean initialNotify) {
    int uid = SimDeviceDataJNI.registerSimDeviceCreatedCallback(prefix, callback, initialNotify);
    return new CallbackStore(uid, SimDeviceDataJNI::cancelSimDeviceCreatedCallback);
  }

  /**
   * Register a callback to be run every time a {@link edu.wpi.first.hal.SimDevice} is
   * freed/destroyed.
   *
   * @param callback the callback
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public static CallbackStore registerDeviceFreedCallback(
      String prefix, SimDeviceCallback callback, boolean initialNotify) {
    int uid = SimDeviceDataJNI.registerSimDeviceFreedCallback(prefix, callback, initialNotify);
    return new CallbackStore(uid, SimDeviceDataJNI::cancelSimDeviceFreedCallback);
  }

  /** Reset all SimDevice data. */
  public static void resetData() {
    SimDeviceDataJNI.resetSimDeviceData();
  }
}
