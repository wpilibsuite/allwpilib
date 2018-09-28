/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

import java.nio.ByteBuffer;
import java.util.function.Consumer;

/**
 * NetworkTables Entry.
 */
public final class NetworkTableEntry {
  /**
   * Flag values (as returned by {@link #getFlags()}).
   */
  public static final int kPersistent = 0x01;

  /**
   * Construct from native handle.
   *
   * @param inst Instance
   * @param handle Native handle
   */
  public NetworkTableEntry(NetworkTableInstance inst, int handle) {
    m_inst = inst;
    m_handle = handle;
  }

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  public boolean isValid() {
    return m_handle != 0;
  }

  /**
   * Gets the native handle for the entry.
   *
   * @return Native handle
   */
  public int getHandle() {
    return m_handle;
  }

  /**
   * Gets the instance for the entry.
   *
   * @return Instance
   */
  public NetworkTableInstance getInstance() {
    return m_inst;
  }

  /**
   * Determines if the entry currently exists.
   *
   * @return True if the entry exists, false otherwise.
   */
  public boolean exists() {
    return NetworkTablesJNI.getType(m_handle) != 0;
  }

  /**
   * Gets the name of the entry (the key).
   *
   * @return the entry's name
   */
  public String getName() {
    return NetworkTablesJNI.getEntryName(m_handle);
  }

  /**
   * Gets the type of the entry.
   *
   * @return the entry's type
   */
  public NetworkTableType getType() {
    return NetworkTableType.getFromInt(NetworkTablesJNI.getType(m_handle));
  }

  /**
   * Returns the flags.
   *
   * @return the flags (bitmask)
   */
  public int getFlags() {
    return NetworkTablesJNI.getEntryFlags(m_handle);
  }

  /**
   * Gets the last time the entry's value was changed.
   *
   * @return Entry last change time
   */
  public long getLastChange() {
    return NetworkTablesJNI.getEntryLastChange(m_handle);
  }

  /**
   * Gets combined information about the entry.
   *
   * @return Entry information
   */
  public EntryInfo getInfo() {
    return NetworkTablesJNI.getEntryInfoHandle(m_inst, m_handle);
  }

  /**
   * Gets the entry's value.
   * Returns a value with type NetworkTableType.kUnassigned if the value
   * does not exist.
   *
   * @return the entry's value
   */
  public NetworkTableValue getValue() {
    return NetworkTablesJNI.getValue(m_handle);
  }

  /**
   * Gets the entry's value as a boolean. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  public boolean getBoolean(boolean defaultValue) {
    return NetworkTablesJNI.getBoolean(m_handle, defaultValue);
  }

  /**
   * Gets the entry's value as a double. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  public double getDouble(double defaultValue) {
    return NetworkTablesJNI.getDouble(m_handle, defaultValue);
  }

  /**
   * Gets the entry's value as a double. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  public Number getNumber(Number defaultValue) {
    return NetworkTablesJNI.getDouble(m_handle, defaultValue.doubleValue());
  }

  /**
   * Gets the entry's value as a string. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  public String getString(String defaultValue) {
    return NetworkTablesJNI.getString(m_handle, defaultValue);
  }

  /**
   * Gets the entry's value as a raw value (byte array). If the entry does not
   * exist or is of different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  public byte[] getRaw(byte[] defaultValue) {
    return NetworkTablesJNI.getRaw(m_handle, defaultValue);
  }

  /**
   * Gets the entry's value as a boolean array. If the entry does not exist
   * or is of different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  public boolean[] getBooleanArray(boolean[] defaultValue) {
    return NetworkTablesJNI.getBooleanArray(m_handle, defaultValue);
  }

  /**
   * Gets the entry's value as a boolean array. If the entry does not exist
   * or is of different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  public Boolean[] getBooleanArray(Boolean[] defaultValue) {
    return NetworkTableValue.fromNative(NetworkTablesJNI.getBooleanArray(m_handle,
        NetworkTableValue.toNative(defaultValue)));
  }

  /**
   * Gets the entry's value as a double array. If the entry does not exist
   * or is of different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  public double[] getDoubleArray(double[] defaultValue) {
    return NetworkTablesJNI.getDoubleArray(m_handle, defaultValue);
  }

  /**
   * Gets the entry's value as a double array. If the entry does not exist
   * or is of different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  public Double[] getDoubleArray(Double[] defaultValue) {
    return NetworkTableValue.fromNative(NetworkTablesJNI.getDoubleArray(m_handle,
        NetworkTableValue.toNative(defaultValue)));
  }

  /**
   * Gets the entry's value as a double array. If the entry does not exist
   * or is of different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  public Number[] getNumberArray(Number[] defaultValue) {
    return NetworkTableValue.fromNative(NetworkTablesJNI.getDoubleArray(m_handle,
        NetworkTableValue.toNative(defaultValue)));
  }

  /**
   * Gets the entry's value as a string array. If the entry does not exist
   * or is of different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  public String[] getStringArray(String[] defaultValue) {
    return NetworkTablesJNI.getStringArray(m_handle, defaultValue);
  }

  /**
   * Checks if a data value is of a type that can be placed in a NetworkTable entry.
   *
   * @param data the data to check
   * @return true if the data can be placed in an entry, false if it cannot
   */
  public static boolean isValidDataType(Object data) {
    return data instanceof Number
        || data instanceof Boolean
        || data instanceof String
        || data instanceof double[]
        || data instanceof Double[]
        || data instanceof Number[]
        || data instanceof boolean[]
        || data instanceof Boolean[]
        || data instanceof String[]
        || data instanceof byte[]
        || data instanceof Byte[];
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   * @throws IllegalArgumentException if the value is not a known type
   */
  public boolean setDefaultValue(Object defaultValue) {
    if (defaultValue instanceof NetworkTableValue) {
      long time = ((NetworkTableValue) defaultValue).getTime();
      Object otherValue = ((NetworkTableValue) defaultValue).getValue();
      switch (((NetworkTableValue) defaultValue).getType()) {
        case kBoolean:
          return NetworkTablesJNI.setDefaultBoolean(m_handle, time,
              ((Boolean) otherValue).booleanValue());
        case kDouble:
          return NetworkTablesJNI.setDefaultDouble(m_handle, time,
              ((Number) otherValue).doubleValue());
        case kString:
          return NetworkTablesJNI.setDefaultString(m_handle, time, (String) otherValue);
        case kRaw:
          return NetworkTablesJNI.setDefaultRaw(m_handle, time, (byte[]) otherValue);
        case kBooleanArray:
          return NetworkTablesJNI.setDefaultBooleanArray(m_handle, time, (boolean[]) otherValue);
        case kDoubleArray:
          return NetworkTablesJNI.setDefaultDoubleArray(m_handle, time, (double[]) otherValue);
        case kStringArray:
          return NetworkTablesJNI.setDefaultStringArray(m_handle, time, (String[]) otherValue);
        case kRpc:
          // TODO
        default:
          return true;
      }
    } else if (defaultValue instanceof Boolean) {
      return setDefaultBoolean((Boolean) defaultValue);
    } else if (defaultValue instanceof Number) {
      return setDefaultNumber((Number) defaultValue);
    } else if (defaultValue instanceof String) {
      return setDefaultString((String) defaultValue);
    } else if (defaultValue instanceof byte[])  {
      return setDefaultRaw((byte[]) defaultValue);
    } else if (defaultValue instanceof boolean[])  {
      return setDefaultBooleanArray((boolean[]) defaultValue);
    } else if (defaultValue instanceof double[])  {
      return setDefaultDoubleArray((double[]) defaultValue);
    } else if (defaultValue instanceof Boolean[])  {
      return setDefaultBooleanArray((Boolean[]) defaultValue);
    } else if (defaultValue instanceof Number[])  {
      return setDefaultNumberArray((Number[]) defaultValue);
    } else if (defaultValue instanceof String[])  {
      return setDefaultStringArray((String[]) defaultValue);
    } else {
      throw new IllegalArgumentException("Value of type " + defaultValue.getClass().getName()
        + " cannot be put into a table");
    }
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  public boolean setDefaultBoolean(boolean defaultValue) {
    return NetworkTablesJNI.setDefaultBoolean(m_handle, 0, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  public boolean setDefaultDouble(double defaultValue) {
    return NetworkTablesJNI.setDefaultDouble(m_handle, 0, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  public boolean setDefaultNumber(Number defaultValue) {
    return NetworkTablesJNI.setDefaultDouble(m_handle, 0, defaultValue.doubleValue());
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  public boolean setDefaultString(String defaultValue) {
    return NetworkTablesJNI.setDefaultString(m_handle, 0, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  public boolean setDefaultRaw(byte[] defaultValue) {
    return NetworkTablesJNI.setDefaultRaw(m_handle, 0, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  public boolean setDefaultBooleanArray(boolean[] defaultValue) {
    return NetworkTablesJNI.setDefaultBooleanArray(m_handle, 0, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  public boolean setDefaultBooleanArray(Boolean[] defaultValue) {
    return NetworkTablesJNI.setDefaultBooleanArray(m_handle,
        0, NetworkTableValue.toNative(defaultValue));
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  public boolean setDefaultDoubleArray(double[] defaultValue) {
    return NetworkTablesJNI.setDefaultDoubleArray(m_handle, 0, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  public boolean setDefaultNumberArray(Number[] defaultValue) {
    return NetworkTablesJNI.setDefaultDoubleArray(m_handle,
        0, NetworkTableValue.toNative(defaultValue));
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  public boolean setDefaultStringArray(String[] defaultValue) {
    return NetworkTablesJNI.setDefaultStringArray(m_handle, 0, defaultValue);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   * @throws IllegalArgumentException if the value is not a known type
   */
  public boolean setValue(Object value) {
    if (value instanceof NetworkTableValue) {
      long time = ((NetworkTableValue) value).getTime();
      Object otherValue = ((NetworkTableValue) value).getValue();
      switch (((NetworkTableValue) value).getType()) {
        case kBoolean:
          return NetworkTablesJNI.setBoolean(m_handle, time, ((Boolean) otherValue).booleanValue(),
              false);
        case kDouble:
          return NetworkTablesJNI.setDouble(m_handle, time, ((Number) otherValue).doubleValue(),
              false);
        case kString:
          return NetworkTablesJNI.setString(m_handle, time, (String) otherValue, false);
        case kRaw:
          return NetworkTablesJNI.setRaw(m_handle, time, (byte[]) otherValue, false);
        case kBooleanArray:
          return NetworkTablesJNI.setBooleanArray(m_handle, time, (boolean[]) otherValue, false);
        case kDoubleArray:
          return NetworkTablesJNI.setDoubleArray(m_handle, time, (double[]) otherValue, false);
        case kStringArray:
          return NetworkTablesJNI.setStringArray(m_handle, time, (String[]) otherValue, false);
        case kRpc:
          // TODO
        default:
          return true;
      }
    } else if (value instanceof Boolean) {
      return setBoolean((Boolean) value);
    } else if (value instanceof Number) {
      return setNumber((Number) value);
    } else if (value instanceof String) {
      return setString((String) value);
    } else if (value instanceof byte[]) {
      return setRaw((byte[]) value);
    } else if (value instanceof boolean[]) {
      return setBooleanArray((boolean[]) value);
    } else if (value instanceof double[]) {
      return setDoubleArray((double[]) value);
    } else if (value instanceof Boolean[]) {
      return setBooleanArray((Boolean[]) value);
    } else if (value instanceof Number[]) {
      return setNumberArray((Number[]) value);
    } else if (value instanceof String[]) {
      return setStringArray((String[]) value);
    } else {
      throw new IllegalArgumentException("Value of type " + value.getClass().getName()
        + " cannot be put into a table");
    }
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  public boolean setBoolean(boolean value) {
    return NetworkTablesJNI.setBoolean(m_handle, 0, value, false);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  public boolean setDouble(double value) {
    return NetworkTablesJNI.setDouble(m_handle, 0, value, false);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  public boolean setNumber(Number value) {
    return NetworkTablesJNI.setDouble(m_handle, 0, value.doubleValue(), false);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  public boolean setString(String value) {
    return NetworkTablesJNI.setString(m_handle, 0, value, false);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  public boolean setRaw(byte[] value) {
    return NetworkTablesJNI.setRaw(m_handle, 0, value, false);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param len the length of the value
   * @return False if the entry exists with a different type
   */
  public boolean setRaw(ByteBuffer value, int len) {
    if (!value.isDirect()) {
      throw new IllegalArgumentException("must be a direct buffer");
    }
    if (value.capacity() < len) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + len);
    }
    return NetworkTablesJNI.setRaw(m_handle, 0, value, len, false);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  public boolean setBooleanArray(boolean[] value) {
    return NetworkTablesJNI.setBooleanArray(m_handle, 0, value, false);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  public boolean setBooleanArray(Boolean[] value) {
    return NetworkTablesJNI.setBooleanArray(m_handle, 0, NetworkTableValue.toNative(value), false);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  public boolean setDoubleArray(double[] value) {
    return NetworkTablesJNI.setDoubleArray(m_handle, 0, value, false);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  public boolean setNumberArray(Number[] value) {
    return NetworkTablesJNI.setDoubleArray(m_handle, 0, NetworkTableValue.toNative(value), false);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  public boolean setStringArray(String[] value) {
    return NetworkTablesJNI.setStringArray(m_handle, 0, value, false);
  }

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   * @throws IllegalArgumentException if the value is not a known type
   */
  public void forceSetValue(Object value) {
    if (value instanceof NetworkTableValue) {
      long time = ((NetworkTableValue) value).getTime();
      Object otherValue = ((NetworkTableValue) value).getValue();
      switch (((NetworkTableValue) value).getType()) {
        case kBoolean:
          NetworkTablesJNI.setBoolean(m_handle, time, ((Boolean) otherValue).booleanValue(), true);
          return;
        case kDouble:
          NetworkTablesJNI.setDouble(m_handle, time, ((Number) otherValue).doubleValue(), true);
          return;
        case kString:
          NetworkTablesJNI.setString(m_handle, time, (String) otherValue, true);
          return;
        case kRaw:
          NetworkTablesJNI.setRaw(m_handle, time, (byte[]) otherValue, true);
          return;
        case kBooleanArray:
          NetworkTablesJNI.setBooleanArray(m_handle, time, (boolean[]) otherValue, true);
          return;
        case kDoubleArray:
          NetworkTablesJNI.setDoubleArray(m_handle, time, (double[]) otherValue, true);
          return;
        case kStringArray:
          NetworkTablesJNI.setStringArray(m_handle, time, (String[]) otherValue, true);
          return;
        case kRpc:
          // TODO
        default:
          return;
      }
    } else if (value instanceof Boolean) {
      forceSetBoolean((Boolean) value);
    } else if (value instanceof Number) {
      forceSetNumber((Number) value);
    } else if (value instanceof String) {
      forceSetString((String) value);
    } else if (value instanceof byte[]) {
      forceSetRaw((byte[]) value);
    } else if (value instanceof boolean[]) {
      forceSetBooleanArray((boolean[]) value);
    } else if (value instanceof double[]) {
      forceSetDoubleArray((double[]) value);
    } else if (value instanceof Boolean[]) {
      forceSetBooleanArray((Boolean[]) value);
    } else if (value instanceof Number[]) {
      forceSetNumberArray((Number[]) value);
    } else if (value instanceof String[]) {
      forceSetStringArray((String[]) value);
    } else {
      throw new IllegalArgumentException("Value of type " + value.getClass().getName()
        + " cannot be put into a table");
    }
  }

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  public void forceSetBoolean(boolean value) {
    NetworkTablesJNI.setBoolean(m_handle, 0, value, true);
  }

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  public void forceSetDouble(double value) {
    NetworkTablesJNI.setDouble(m_handle, 0, value, true);
  }

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  public void forceSetNumber(Number value) {
    NetworkTablesJNI.setDouble(m_handle, 0, value.doubleValue(), true);
  }

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  public void forceSetString(String value) {
    NetworkTablesJNI.setString(m_handle, 0, value, true);
  }

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  public void forceSetRaw(byte[] value) {
    NetworkTablesJNI.setRaw(m_handle, 0, value, true);
  }

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  public void forceSetBooleanArray(boolean[] value) {
    NetworkTablesJNI.setBooleanArray(m_handle, 0, value, true);
  }

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  public void forceSetBooleanArray(Boolean[] value) {
    NetworkTablesJNI.setBooleanArray(m_handle, 0, NetworkTableValue.toNative(value), true);
  }

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  public void forceSetDoubleArray(double[] value) {
    NetworkTablesJNI.setDoubleArray(m_handle, 0, value, true);
  }

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  public void forceSetNumberArray(Number[] value) {
    NetworkTablesJNI.setDoubleArray(m_handle, 0, NetworkTableValue.toNative(value), true);
  }

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  public void forceSetStringArray(String[] value) {
    NetworkTablesJNI.setStringArray(m_handle, 0, value, true);
  }

  /**
   * Sets flags.
   *
   * @param flags the flags to set (bitmask)
   */
  public void setFlags(int flags) {
    NetworkTablesJNI.setEntryFlags(m_handle, getFlags() | flags);
  }

  /**
   * Clears flags.
   *
   * @param flags the flags to clear (bitmask)
   */
  public void clearFlags(int flags) {
    NetworkTablesJNI.setEntryFlags(m_handle, getFlags() & ~flags);
  }

  /**
   * Make value persistent through program restarts.
   */
  public void setPersistent() {
    setFlags(kPersistent);
  }

  /**
   * Stop making value persistent through program restarts.
   */
  public void clearPersistent() {
    clearFlags(kPersistent);
  }

  /**
   * Returns whether the value is persistent through program restarts.
   *
   * @return True if the value is persistent.
   */
  public boolean isPersistent() {
    return (getFlags() & kPersistent) != 0;
  }

  /**
   * Deletes the entry.
   */
  public void delete() {
    NetworkTablesJNI.deleteEntry(m_handle);
  }

  /**
   * Create a callback-based RPC entry point.  Only valid to use on the server.
   * The callback function will be called when the RPC is called.
   * This function creates RPC version 0 definitions (raw data in and out).
   *
   * @param callback  callback function
   */
  public void createRpc(Consumer<RpcAnswer> callback) {
    m_inst.createRpc(this, callback);
  }

  /**
   * Call a RPC function.  May be used on either the client or server.
   * This function is non-blocking.  Either {@link RpcCall#getResult()} or
   * {@link RpcCall#cancelResult()} must be called on the return value to either
   * get or ignore the result of the call.
   *
   * @param params      parameter
   * @return RPC call object.
   */
  public RpcCall callRpc(byte[] params) {
    return new RpcCall(this, NetworkTablesJNI.callRpc(m_handle, params));
  }

  /**
   * Add a listener for changes to the entry.
   *
   * @param listener the listener to add
   * @param flags bitmask specifying desired notifications
   * @return listener handle
   */
  public int addListener(Consumer<EntryNotification> listener, int flags) {
    return m_inst.addEntryListener(this, listener, flags);
  }

  /**
   * Remove a listener from receiving entry events.
   *
   * @param listener the listener to be removed
   */
  public void removeListener(int listener) {
    m_inst.removeEntryListener(listener);
  }

  @Override
  public boolean equals(Object other) {
    if (other == this) {
      return true;
    }
    if (!(other instanceof NetworkTableEntry)) {
      return false;
    }

    return m_handle == ((NetworkTableEntry) other).m_handle;
  }

  @Override
  public int hashCode() {
    return m_handle;
  }

  private NetworkTableInstance m_inst;
  private int m_handle;
}
