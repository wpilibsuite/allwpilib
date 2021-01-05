// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * A wrapper around a simulator device handle.
 *
 * <p>Teams: if you are using this class, you are likely confusing it for {@link
 * edu.wpi.first.wpilibj.simulation.SimDeviceSim}.
 *
 * <p>Vendors: This class should be used from inside the device class to define the
 * properties/fields of the device. Use {@link #create} to get a SimDevice object, then use {@link
 * #createDouble(String, Direction, double)} or similar to define the device's fields. See {@link
 * edu.wpi.first.wpilibj.ADXRS450_Gyro} for an example implementation.
 */
public class SimDevice implements AutoCloseable {
  public enum Direction {
    kInput(SimDeviceJNI.kInput),
    kOutput(SimDeviceJNI.kOutput),
    kBidir(SimDeviceJNI.kBidir);

    public final int m_value;

    Direction(int value) {
      m_value = value;
    }
  }

  /**
   * Creates a simulated device.
   *
   * <p>The device name must be unique. Returns null if the device name already exists. If multiple
   * instances of the same device are desired, recommend appending the instance/unique identifer in
   * brackets to the base name, e.g. "device[1]".
   *
   * <p>null is returned if not in simulation.
   *
   * @param name device name
   * @return simulated device object
   */
  public static SimDevice create(String name) {
    int handle = SimDeviceJNI.createSimDevice(name);
    if (handle <= 0) {
      return null;
    }
    return new SimDevice(handle);
  }

  /**
   * Creates a simulated device.
   *
   * <p>The device name must be unique. Returns null if the device name already exists. This is a
   * convenience method that appends index in brackets to the device name, e.g. passing index=1
   * results in "device[1]" for the device name.
   *
   * <p>null is returned if not in simulation.
   *
   * @param name device name
   * @param index device index number to append to name
   * @return simulated device object
   */
  public static SimDevice create(String name, int index) {
    return create(name + "[" + index + "]");
  }

  /**
   * Creates a simulated device.
   *
   * <p>The device name must be unique. Returns null if the device name already exists. This is a
   * convenience method that appends index and channel in brackets to the device name, e.g. passing
   * index=1 and channel=2 results in "device[1,2]" for the device name.
   *
   * <p>null is returned if not in simulation.
   *
   * @param name device name
   * @param index device index number to append to name
   * @param channel device channel number to append to name
   * @return simulated device object
   */
  public static SimDevice create(String name, int index, int channel) {
    return create(name + "[" + index + "," + channel + "]");
  }

  /**
   * Wraps a simulated device handle as returned by SimDeviceJNI.createSimDevice().
   *
   * @param handle simulated device handle
   */
  public SimDevice(int handle) {
    m_handle = handle;
  }

  @Override
  public void close() {
    SimDeviceJNI.freeSimDevice(m_handle);
  }

  /**
   * Get the internal device handle.
   *
   * @return internal handle
   */
  public int getNativeHandle() {
    return m_handle;
  }

  /**
   * Creates a value on the simulated device.
   *
   * <p>Returns null if not in simulation.
   *
   * @param name value name
   * @param readonly if the value should not be written from simulation side
   * @param initialValue initial value
   * @return simulated value object
   * @deprecated Use direction function instead
   */
  @Deprecated
  public SimValue createValue(String name, boolean readonly, HALValue initialValue) {
    return createValue(name, readonly ? Direction.kOutput : Direction.kInput, initialValue);
  }

  /**
   * Creates a value on the simulated device.
   *
   * <p>Returns null if not in simulation.
   *
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param initialValue initial value
   * @return simulated value object
   */
  public SimValue createValue(String name, Direction direction, HALValue initialValue) {
    int handle = SimDeviceJNI.createSimValue(m_handle, name, direction.m_value, initialValue);
    if (handle <= 0) {
      return null;
    }
    return new SimValue(handle);
  }

  /**
   * Creates a double value on the simulated device.
   *
   * <p>Returns null if not in simulation.
   *
   * @param name value name
   * @param readonly if the value should not be written from simulation side
   * @param initialValue initial value
   * @return simulated double value object
   * @deprecated Use direction function instead
   */
  @Deprecated
  public SimDouble createDouble(String name, boolean readonly, double initialValue) {
    return createDouble(name, readonly ? Direction.kOutput : Direction.kInput, initialValue);
  }

  /**
   * Creates a double value on the simulated device.
   *
   * <p>Returns null if not in simulation.
   *
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param initialValue initial value
   * @return simulated double value object
   */
  public SimDouble createDouble(String name, Direction direction, double initialValue) {
    int handle = SimDeviceJNI.createSimValueDouble(m_handle, name, direction.m_value, initialValue);
    if (handle <= 0) {
      return null;
    }
    return new SimDouble(handle);
  }

  /**
   * Creates an enumerated value on the simulated device.
   *
   * <p>Enumerated values are always in the range 0 to numOptions-1.
   *
   * <p>Returns null if not in simulation.
   *
   * @param name value name
   * @param readonly if the value should not be written from simulation side
   * @param options array of option descriptions
   * @param initialValue initial value (selection)
   * @return simulated enum value object
   * @deprecated Use direction function instead
   */
  @Deprecated
  public SimEnum createEnum(String name, boolean readonly, String[] options, int initialValue) {
    return createEnum(name, readonly ? Direction.kOutput : Direction.kInput, options, initialValue);
  }

  /**
   * Creates an enumerated value on the simulated device.
   *
   * <p>Enumerated values are always in the range 0 to numOptions-1.
   *
   * <p>Returns null if not in simulation.
   *
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param options array of option descriptions
   * @param initialValue initial value (selection)
   * @return simulated enum value object
   */
  public SimEnum createEnum(String name, Direction direction, String[] options, int initialValue) {
    int handle =
        SimDeviceJNI.createSimValueEnum(m_handle, name, direction.m_value, options, initialValue);
    if (handle <= 0) {
      return null;
    }
    return new SimEnum(handle);
  }

  /**
   * Creates an enumerated value on the simulated device with double values.
   *
   * <p>Enumerated values are always in the range 0 to numOptions-1.
   *
   * <p>Returns null if not in simulation.
   *
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param options array of option descriptions
   * @param optionValues array of option values (must be the same size as options)
   * @param initialValue initial value (selection)
   * @return simulated enum value object
   */
  public SimEnum createEnumDouble(
      String name, Direction direction, String[] options, double[] optionValues, int initialValue) {
    int handle =
        SimDeviceJNI.createSimValueEnumDouble(
            m_handle, name, direction.m_value, options, optionValues, initialValue);
    if (handle <= 0) {
      return null;
    }
    return new SimEnum(handle);
  }

  /**
   * Creates a boolean value on the simulated device.
   *
   * <p>Returns null if not in simulation.
   *
   * @param name value name
   * @param readonly if the value should not be written from simulation side
   * @param initialValue initial value
   * @return simulated boolean value object
   * @deprecated Use direction function instead
   */
  @Deprecated
  public SimBoolean createBoolean(String name, boolean readonly, boolean initialValue) {
    return createBoolean(name, readonly ? Direction.kOutput : Direction.kInput, initialValue);
  }

  /**
   * Creates a boolean value on the simulated device.
   *
   * <p>Returns null if not in simulation.
   *
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param initialValue initial value
   * @return simulated boolean value object
   */
  public SimBoolean createBoolean(String name, Direction direction, boolean initialValue) {
    int handle =
        SimDeviceJNI.createSimValueBoolean(m_handle, name, direction.m_value, initialValue);
    if (handle <= 0) {
      return null;
    }
    return new SimBoolean(handle);
  }

  private final int m_handle;
}
