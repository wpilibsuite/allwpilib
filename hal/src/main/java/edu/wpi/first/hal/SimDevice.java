/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

/**
 * A wrapper around a simulator device handle.
 */
public class SimDevice implements AutoCloseable {
  public static enum Direction {
    kInput(SimDeviceJNI.kInput),
    kOutput(SimDeviceJNI.kOutput),
    kBidir(SimDeviceJNI.kBidir);

    public final int value;

    private Direction(int value) {
      this.value = value;
    }
  }

  /**
   * Creates a simulated device.
   *
   * <p>The device name must be unique.  Returns null if the device name
   * already exists.  If multiple instances of the same device are desired,
   * recommend appending the instance/unique identifer in brackets to the base
   * name, e.g. "device[1]".
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
   * <p>The device name must be unique.  Returns null if the device name
   * already exists.  This is a convenience method that appends index in
   * brackets to the device name, e.g. passing index=1 results in "device[1]"
   * for the device name.
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
   * <p>The device name must be unique.  Returns null if the device name
   * already exists.  This is a convenience method that appends index and
   * channel in brackets to the device name, e.g. passing index=1 and channel=2
   * results in "device[1,2]" for the device name.
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
   *
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
    int handle = SimDeviceJNI.createSimValue(m_handle, name, direction.value, initialValue);
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
   *
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
    int handle = SimDeviceJNI.createSimValueDouble(m_handle, name, direction.value, initialValue);
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
   *
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
    int handle = SimDeviceJNI.createSimValueEnum(m_handle, name, direction.value, options, initialValue);
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
  public SimEnum createEnumDouble(String name, Direction direction, String[] options, double[] optionValues,
      int initialValue) {
    int handle = SimDeviceJNI.createSimValueEnumDouble(m_handle, name, direction.value, options, optionValues,
        initialValue);
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
   *
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
    int handle = SimDeviceJNI.createSimValueBoolean(m_handle, name, direction.value, initialValue);
    if (handle <= 0) {
      return null;
    }
    return new SimBoolean(handle);
  }

  private final int m_handle;
}
