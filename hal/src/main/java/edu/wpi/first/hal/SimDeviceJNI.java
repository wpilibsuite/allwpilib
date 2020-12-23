/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

public class SimDeviceJNI extends JNIWrapper {
  public static final int kInput = 0;
  public static final int kOutput = 1;
  public static final int kBidir = 2;

  /**
   * Creates a simulated device.
   *
   * <p>The device name must be unique.  0 is returned if the device name
   * already exists.  If multiple instances of the same device are desired,
   * recommend appending the instance/unique identifer in brackets to the base
   * name, e.g. "device[1]".
   *
   * <p>0 is returned if not in simulation.
   *
   * @param name device name
   * @return simulated device handle
   */
  public static native int createSimDevice(String name);

  /**
   * Frees a simulated device.
   *
   * <p>This also allows the same device name to be used again.
   * This also frees all the simulated values created on the device.
   *
   * @param handle simulated device handle
   */
  public static native void freeSimDevice(int handle);

  private static native int createSimValueNative(int device, String name, int direction,
      int type, long value1, double value2);

  /**
   * Creates a value on a simulated device.
   *
   * <p>Returns 0 if not in simulation; this can be used to avoid calls
   * to Set/Get functions.
   *
   * @param device simulated device handle
   * @param name value name
   * @param readonly if the value should not be written from simulation side
   * @param initialValue initial value
   * @return simulated value handle
   *
   * @deprecated Use direction-taking function instead
   */
  @Deprecated
  public static int createSimValue(int device, String name, boolean readonly,
      HALValue initialValue) {
    return createSimValueNative(device, name, readonly ? kOutput : kInput, initialValue.getType(),
        initialValue.getNativeLong(), initialValue.getNativeDouble());
  }

  /**
   * Creates a value on a simulated device.
   *
   * <p>Returns 0 if not in simulation; this can be used to avoid calls
   * to Set/Get functions.
   *
   * @param device simulated device handle
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param initialValue initial value
   * @return simulated value handle
   */
  public static int createSimValue(int device, String name, int direction,
      HALValue initialValue) {
    return createSimValueNative(device, name, direction, initialValue.getType(),
        initialValue.getNativeLong(), initialValue.getNativeDouble());
  }

  /**
   * Creates a double value on a simulated device.
   *
   * <p>Returns 0 if not in simulation; this can be used to avoid calls
   * to Set/Get functions.
   *
   * @param device simulated device handle
   * @param name value name
   * @param readonly if the value should not be written from simulation side
   * @param initialValue initial value
   * @return simulated value handle
   *
   * @deprecated Use direction-taking function instead
   */
  @Deprecated
  public static int createSimValueDouble(int device, String name, boolean readonly,
      double initialValue) {
    return createSimValueNative(device, name, readonly ? kOutput : kInput, HALValue.kDouble, 0, initialValue);
  }

  /**
   * Creates a double value on a simulated device.
   *
   * <p>Returns 0 if not in simulation; this can be used to avoid calls
   * to Set/Get functions.
   *
   * @param device simulated device handle
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param initialValue initial value
   * @return simulated value handle
   */
  public static int createSimValueDouble(int device, String name, int direction,
      double initialValue) {
    return createSimValueNative(device, name, direction, HALValue.kDouble, 0, initialValue);
  }

  /**
   * Creates an enumerated value on a simulated device.
   *
   * <p>Enumerated values are always in the range 0 to numOptions-1.
   *
   * <p>Returns 0 if not in simulation; this can be used to avoid calls
   * to Set/Get functions.
   *
   * @param device simulated device handle
   * @param name value name
   * @param readonly if the value should not be written from simulation side
   * @param options array of option descriptions
   * @param initialValue initial value (selection)
   * @return simulated value handle
   *
   * @deprecated Use direction-taking function instead
   */
  @Deprecated
  public static int createSimValueEnum(int device, String name, boolean readonly,
      String[] options, int initialValue) {
    return createSimValueEnum(device, name, readonly ? kOutput : kInput, options, initialValue);
  }

  /**
   * Creates an enumerated value on a simulated device.
   *
   * <p>Enumerated values are always in the range 0 to numOptions-1.
   *
   * <p>Returns 0 if not in simulation; this can be used to avoid calls
   * to Set/Get functions.
   *
   * @param device simulated device handle
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param options array of option descriptions
   * @param initialValue initial value (selection)
   * @return simulated value handle
   */
  public static native int createSimValueEnum(int device, String name, int direction,
      String[] options, int initialValue);

  /**
   * Creates an enumerated value on a simulated device with double values.
   *
   * <p>Enumerated values are always in the range 0 to numOptions-1.
   *
   * <p>Returns 0 if not in simulation; this can be used to avoid calls
   * to Set/Get functions.
   *
   * @param device simulated device handle
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param options array of option descriptions
   * @param optionValues array of option values (must be the same size as options)
   * @param initialValue initial value (selection)
   * @return simulated value handle
   */
  public static native int createSimValueEnumDouble(int device, String name, int direction,
      String[] options, double[] optionValues, int initialValue);

  /**
   * Creates a boolean value on a simulated device.
   *
   * <p>Returns 0 if not in simulation; this can be used to avoid calls
   * to Set/Get functions.
   *
   * @param device simulated device handle
   * @param name value name
   * @param readonly if the value should not be written from simulation side
   * @param initialValue initial value
   * @return simulated value handle
   *
   * @deprecated Use direction-taking function instead
   */
  @Deprecated
  public static int createSimValueBoolean(int device, String name, boolean readonly,
      boolean initialValue) {
    return createSimValueNative(device, name, readonly ? kOutput : kInput, HALValue.kBoolean,
        initialValue ? 1 : 0, 0.0);
  }

  /**
   * Creates a boolean value on a simulated device.
   *
   * <p>Returns 0 if not in simulation; this can be used to avoid calls
   * to Set/Get functions.
   *
   * @param device simulated device handle
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param initialValue initial value
   * @return simulated value handle
   */
  public static int createSimValueBoolean(int device, String name, int direction,
      boolean initialValue) {
    return createSimValueNative(device, name, direction, HALValue.kBoolean,
        initialValue ? 1 : 0, 0.0);
  }

  /**
   * Gets a simulated value.
   *
   * @param handle simulated value handle
   * @return The current value
   */
  public static native HALValue getSimValue(int handle);

  /**
   * Gets a simulated value (double).
   *
   * @param handle simulated value handle
   * @return The current value
   */
  public static native double getSimValueDouble(int handle);

  /**
   * Gets a simulated value (enum).
   *
   * @param handle simulated value handle
   * @return The current value
   */
  public static native int getSimValueEnum(int handle);

  /**
   * Gets a simulated value (boolean).
   *
   * @param handle simulated value handle
   * @return The current value
   */
  public static native boolean getSimValueBoolean(int handle);

  private static native void setSimValueNative(int handle, int type, long value1, double value2);

  /**
   * Sets a simulated value.
   *
   * @param handle simulated value handle
   * @param value the value to set
   */
  public static void setSimValue(int handle, HALValue value) {
    setSimValueNative(handle, value.getType(), value.getNativeLong(), value.getNativeDouble());
  }

  /**
   * Sets a simulated value (double).
   *
   * @param handle simulated value handle
   * @param value the value to set
   */
  public static void setSimValueDouble(int handle, double value) {
    setSimValueNative(handle, HALValue.kDouble, 0, value);
  }

  /**
   * Sets a simulated value (enum).
   *
   * @param handle simulated value handle
   * @param value the value to set
   */
  public static void setSimValueEnum(int handle, int value) {
    setSimValueNative(handle, HALValue.kEnum, value, 0.0);
  }

  /**
   * Sets a simulated value (boolean).
   *
   * @param handle simulated value handle
   * @param value the value to set
   */
  public static void setSimValueBoolean(int handle, boolean value) {
    setSimValueNative(handle, HALValue.kBoolean, value ? 1 : 0, 0.0);
  }
}
