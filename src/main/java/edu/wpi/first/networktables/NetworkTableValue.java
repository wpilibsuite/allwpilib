/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

import java.util.Objects;

/**
 * A network table entry value.
 */
public final class NetworkTableValue {
  NetworkTableValue(NetworkTableType type, Object value, long time) {
    m_type = type;
    m_value = value;
    m_time = time;
  }

  NetworkTableValue(NetworkTableType type, Object value) {
    this(type, value, NetworkTablesJNI.now());
  }

  NetworkTableValue(int type, Object value, long time) {
    this(NetworkTableType.getFromInt(type), value, time);
  }

  /**
   * Get the data type.
   * @return The type.
   */
  public NetworkTableType getType() {
    return m_type;
  }

  /**
   * Get the data value stored.
   * @return The type.
   */
  public Object getValue() {
    return m_value;
  }

  /**
   * Get the creation time of the value.
   * @return The time, in the units returned by NetworkTablesJNI.now().
   */
  public long getTime() {
    return m_time;
  }

  /*
   * Type Checkers
   */

  /**
   * Determine if entry value contains a value or is unassigned.
   * @return True if the entry value contains a value.
   */
  public boolean isValid() {
    return m_type != NetworkTableType.kUnassigned;
  }

  /**
   * Determine if entry value contains a boolean.
   * @return True if the entry value is of boolean type.
   */
  public boolean isBoolean() {
    return m_type == NetworkTableType.kBoolean;
  }

  /**
   * Determine if entry value contains a double.
   * @return True if the entry value is of double type.
   */
  public boolean isDouble() {
    return m_type == NetworkTableType.kDouble;
  }

  /**
   * Determine if entry value contains a string.
   * @return True if the entry value is of string type.
   */
  public boolean isString() {
    return m_type == NetworkTableType.kString;
  }

  /**
   * Determine if entry value contains a raw.
   * @return True if the entry value is of raw type.
   */
  public boolean isRaw() {
    return m_type == NetworkTableType.kRaw;
  }

  /**
   * Determine if entry value contains a rpc definition.
   * @return True if the entry value is of rpc definition type.
   */
  public boolean isRpc() {
    return m_type == NetworkTableType.kRpc;
  }

  /**
   * Determine if entry value contains a boolean array.
   * @return True if the entry value is of boolean array type.
   */
  public boolean isBooleanArray() {
    return m_type == NetworkTableType.kBooleanArray;
  }

  /**
   * Determine if entry value contains a double array.
   * @return True if the entry value is of double array type.
   */
  public boolean isDoubleArray() {
    return m_type == NetworkTableType.kDoubleArray;
  }

  /**
   * Determine if entry value contains a string array.
   * @return True if the entry value is of string array type.
   */
  public boolean isStringArray() {
    return m_type == NetworkTableType.kStringArray;
  }

  /*
   * Type-Safe Getters
   */

  /**
   * Get the entry's boolean value.
   * @throws ClassCastException if the entry value is not of boolean type.
   * @return The boolean value.
   */
  public boolean getBoolean() {
    if (m_type != NetworkTableType.kBoolean) {
      throw new ClassCastException("cannot convert " + m_type + " to boolean");
    }
    return ((Boolean)m_value).booleanValue();
  }

  /**
   * Get the entry's double value.
   * @throws ClassCastException if the entry value is not of double type.
   * @return The double value.
   */
  public double getDouble() {
    if (m_type != NetworkTableType.kDouble) {
      throw new ClassCastException("cannot convert " + m_type + " to double");
    }
    return ((Number)m_value).doubleValue();
  }

  /**
   * Get the entry's string value.
   * @throws ClassCastException if the entry value is not of string type.
   * @return The string value.
   */
  public String getString() {
    if (m_type != NetworkTableType.kString) {
      throw new ClassCastException("cannot convert " + m_type + " to string");
    }
    return (String)m_value;
  }

  /**
   * Get the entry's raw value.
   * @throws ClassCastException if the entry value is not of raw type.
   * @return The raw value.
   */
  public byte[] getRaw() {
    if (m_type != NetworkTableType.kRaw) {
      throw new ClassCastException("cannot convert " + m_type + " to raw");
    }
    return (byte[])m_value;
  }

  /**
   * Get the entry's rpc definition value.
   * @throws ClassCastException if the entry value is not of rpc definition type.
   * @return The rpc definition value.
   */
  public byte[] getRpc() {
    if (m_type != NetworkTableType.kRpc) {
      throw new ClassCastException("cannot convert " + m_type + " to rpc");
    }
    return (byte[])m_value;
  }

  /**
   * Get the entry's boolean array value.
   * @throws ClassCastException if the entry value is not of boolean array type.
   * @return The boolean array value.
   */
  public boolean[] getBooleanArray() {
    if (m_type != NetworkTableType.kBooleanArray) {
      throw new ClassCastException("cannot convert " + m_type + " to boolean array");
    }
    return (boolean[])m_value;
  }

  /**
   * Get the entry's double array value.
   * @throws ClassCastException if the entry value is not of double array type.
   * @return The double array value.
   */
  public double[] getDoubleArray() {
    if (m_type != NetworkTableType.kDoubleArray) {
      throw new ClassCastException("cannot convert " + m_type + " to double array");
    }
    return (double[])m_value;
  }

  /**
   * Get the entry's string array value.
   * @throws ClassCastException if the entry value is not of string array type.
   * @return The string array value.
   */
  public String[] getStringArray() {
    if (m_type != NetworkTableType.kStringArray) {
      throw new ClassCastException("cannot convert " + m_type + " to string array");
    }
    return (String[])m_value;
  }

  /*
   * Factory functions.
   */

  /**
   * Creates a boolean entry value.
   * @param value the value
   * @return The entry value
   */
  public static NetworkTableValue makeBoolean(boolean value) {
    return new NetworkTableValue(NetworkTableType.kBoolean, new Boolean(value));
  }

  /**
   * Creates a boolean entry value.
   * @param value the value
   * @param time the creation time to use (instead of the current time)
   * @return The entry value
   */
  public static NetworkTableValue makeBoolean(boolean value, long time) {
    return new NetworkTableValue(NetworkTableType.kBoolean, new Boolean(value), time);
  }

  /**
   * Creates a double entry value.
   * @param value the value
   * @return The entry value
   */
  public static NetworkTableValue makeDouble(double value) {
    return new NetworkTableValue(NetworkTableType.kDouble, new Double(value));
  }

  /**
   * Creates a double entry value.
   * @param value the value
   * @param time the creation time to use (instead of the current time)
   * @return The entry value
   */
  public static NetworkTableValue makeDouble(double value, long time) {
    return new NetworkTableValue(NetworkTableType.kDouble, new Double(value), time);
  }

  /**
   * Creates a string entry value.
   * @param value the value
   * @return The entry value
   */
  public static NetworkTableValue makeString(String value) {
    return new NetworkTableValue(NetworkTableType.kString, value);
  }

  /**
   * Creates a string entry value.
   * @param value the value
   * @param time the creation time to use (instead of the current time)
   * @return The entry value
   */
  public static NetworkTableValue makeString(String value, long time) {
    return new NetworkTableValue(NetworkTableType.kString, value, time);
  }

  /**
   * Creates a raw entry value.
   * @param value the value
   * @return The entry value
   */
  public static NetworkTableValue makeRaw(byte[] value) {
    return new NetworkTableValue(NetworkTableType.kRaw, value);
  }

  /**
   * Creates a raw entry value.
   * @param value the value
   * @param time the creation time to use (instead of the current time)
   * @return The entry value
   */
  public static NetworkTableValue makeRaw(byte[] value, long time) {
    return new NetworkTableValue(NetworkTableType.kRaw, value, time);
  }

  /**
   * Creates a rpc entry value.
   * @param value the value
   * @return The entry value
   */
  public static NetworkTableValue makeRpc(byte[] value) {
    return new NetworkTableValue(NetworkTableType.kRpc, value);
  }

  /**
   * Creates a rpc entry value.
   * @param value the value
   * @param time the creation time to use (instead of the current time)
   * @return The entry value
   */
  public static NetworkTableValue makeRpc(byte[] value, long time) {
    return new NetworkTableValue(NetworkTableType.kRpc, value, time);
  }

  /**
   * Creates a boolean array entry value.
   * @param value the value
   * @return The entry value
   */
  public static NetworkTableValue makeBooleanArray(boolean[] value) {
    return new NetworkTableValue(NetworkTableType.kBooleanArray, value);
  }

  /**
   * Creates a boolean array entry value.
   * @param value the value
   * @param time the creation time to use (instead of the current time)
   * @return The entry value
   */
  public static NetworkTableValue makeBooleanArray(boolean[] value, long time) {
    return new NetworkTableValue(NetworkTableType.kBooleanArray, value, time);
  }

  /**
   * Creates a boolean array entry value.
   * @param value the value
   * @return The entry value
   */
  public static NetworkTableValue makeBooleanArray(Boolean[] value) {
    return new NetworkTableValue(NetworkTableType.kBooleanArray, toNative(value));
  }

  /**
   * Creates a boolean array entry value.
   * @param value the value
   * @param time the creation time to use (instead of the current time)
   * @return The entry value
   */
  public static NetworkTableValue makeBooleanArray(Boolean[] value, long time) {
    return new NetworkTableValue(NetworkTableType.kBooleanArray, toNative(value), time);
  }

  /**
   * Creates a double array entry value.
   * @param value the value
   * @return The entry value
   */
  public static NetworkTableValue makeDoubleArray(double[] value) {
    return new NetworkTableValue(NetworkTableType.kDoubleArray, value);
  }

  /**
   * Creates a double array entry value.
   * @param value the value
   * @param time the creation time to use (instead of the current time)
   * @return The entry value
   */
  public static NetworkTableValue makeDoubleArray(double[] value, long time) {
    return new NetworkTableValue(NetworkTableType.kDoubleArray, value, time);
  }

  /**
   * Creates a double array entry value.
   * @param value the value
   * @return The entry value
   */
  public static NetworkTableValue makeDoubleArray(Number[] value) {
    return new NetworkTableValue(NetworkTableType.kDoubleArray, toNative(value));
  }

  /**
   * Creates a double array entry value.
   * @param value the value
   * @param time the creation time to use (instead of the current time)
   * @return The entry value
   */
  public static NetworkTableValue makeDoubleArray(Number[] value, long time) {
    return new NetworkTableValue(NetworkTableType.kDoubleArray, toNative(value), time);
  }

  /**
   * Creates a string array entry value.
   * @param value the value
   * @return The entry value
   */
  public static NetworkTableValue makeStringArray(String[] value) {
    return new NetworkTableValue(NetworkTableType.kStringArray, value);
  }

  /**
   * Creates a string array entry value.
   * @param value the value
   * @param time the creation time to use (instead of the current time)
   * @return The entry value
   */
  public static NetworkTableValue makeStringArray(String[] value, long time) {
    return new NetworkTableValue(NetworkTableType.kStringArray, value, time);
  }

  @Override
  public boolean equals(Object o) {
    if (o == this) {
      return true;
    }
    if (!(o instanceof NetworkTableValue)) {
      return false;
    }
    NetworkTableValue other = (NetworkTableValue) o;
    return m_type == other.m_type && m_value.equals(other.m_value);
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_type, m_value);
  }

  static boolean[] toNative(Boolean[] arr) {
    boolean[] out = new boolean[arr.length];
    for (int i = 0; i < arr.length; i++)
      out[i] = arr[i];
    return out;
  }

  static double[] toNative(Number[] arr) {
    double[] out = new double[arr.length];
    for (int i = 0; i < arr.length; i++)
      out[i] = arr[i].doubleValue();
    return out;
  }

  static Boolean[] fromNative(boolean[] arr) {
    Boolean[] out = new Boolean[arr.length];
    for (int i = 0; i < arr.length; i++)
      out[i] = arr[i];
    return out;
  }

  static Double[] fromNative(double[] arr) {
    Double[] out = new Double[arr.length];
    for (int i = 0; i < arr.length; i++)
      out[i] = arr[i];
    return out;
  }

  private NetworkTableType m_type;
  private Object m_value;
  private long m_time;
}
