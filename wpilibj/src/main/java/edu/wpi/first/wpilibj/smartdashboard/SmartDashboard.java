/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.smartdashboard;

import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.HLUsageReporting;
import edu.wpi.first.wpilibj.Sendable;

/**
 * The {@link SmartDashboard} class is the bridge between robot programs and the SmartDashboard on
 * the laptop.
 *
 * <p>When a value is put into the SmartDashboard here, it pops up on the SmartDashboard on the
 * laptop. Users can put values into and get values from the SmartDashboard.
 */
public class SmartDashboard {
  /**
   * The {@link NetworkTable} used by {@link SmartDashboard}.
   */
  private static final NetworkTable table =
      NetworkTableInstance.getDefault().getTable("SmartDashboard");

  private static class Data {
    Data(Sendable sendable) {
      m_sendable = sendable;
    }

    final Sendable m_sendable;
    final SendableBuilderImpl m_builder = new SendableBuilderImpl();
  }

  /**
   * A table linking tables in the SmartDashboard to the {@link Sendable} objects they
   * came from.
   */
  private static final Map<String, Data> tablesToData = new HashMap<>();

  static {
    HLUsageReporting.reportSmartDashboard();
  }

  /**
   * Maps the specified key to the specified value in this table. The key can not be null. The value
   * can be retrieved by calling the get method with a key that is equal to the original key.
   *
   * @param key  the key
   * @param data the value
   * @throws IllegalArgumentException If key is null
   */
  public static synchronized void putData(String key, Sendable data) {
    Data sddata = tablesToData.get(key);
    if (sddata == null || sddata.m_sendable != data) {
      if (sddata != null) {
        sddata.m_builder.stopListeners();
      }
      sddata = new Data(data);
      tablesToData.put(key, sddata);
      NetworkTable dataTable = table.getSubTable(key);
      sddata.m_builder.setTable(dataTable);
      data.initSendable(sddata.m_builder);
      sddata.m_builder.updateTable();
      sddata.m_builder.startListeners();
      dataTable.getEntry(".name").setString(key);
    }
  }

  /**
   * Maps the specified key (where the key is the name of the {@link NamedSendable}
   * to the specified value in this table. The value can be retrieved by
   * calling the get method with a key that is equal to the original key.
   *
   * @param value the value
   * @throws IllegalArgumentException If key is null
   */
  public static void putData(Sendable value) {
    putData(value.getName(), value);
  }

  /**
   * Returns the value at the specified key.
   *
   * @param key the key
   * @return the value
   * @throws IllegalArgumentException  if the key is null
   */
  public static synchronized Sendable getData(String key) {
    Data data = tablesToData.get(key);
    if (data == null) {
      throw new IllegalArgumentException("SmartDashboard data does not exist: " + key);
    } else {
      return data.m_sendable;
    }
  }

  /**
   * Gets the entry for the specified key.
   * @param key the key name
   * @return Network table entry.
   */
  public static NetworkTableEntry getEntry(String key) {
    return table.getEntry(key);
  }

  /**
   * Checks the table and tells if it contains the specified key.
   *
   * @param key the key to search for
   * @return true if the table as a value assigned to the given key
   */
  public static boolean containsKey(String key) {
    return table.containsKey(key);
  }

  /**
   * Get the keys stored in the SmartDashboard table of NetworkTables.
   *
   * @param types bitmask of types; 0 is treated as a "don't care".
   * @return keys currently in the table
   */
  public static Set<String> getKeys(int types) {
    return table.getKeys(types);
  }

  /**
   * Get the keys stored in the SmartDashboard table of NetworkTables.
   *
   * @return keys currently in the table.
   */
  public static Set<String> getKeys() {
    return table.getKeys();
  }

  /**
   * Makes a key's value persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  public static void setPersistent(String key) {
    getEntry(key).setPersistent();
  }

  /**
   * Stop making a key's value persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  public static void clearPersistent(String key) {
    getEntry(key).clearPersistent();
  }

  /**
   * Returns whether the value is persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   * @return True if the value is persistent.
   */
  public static boolean isPersistent(String key) {
    return getEntry(key).isPersistent();
  }

  /**
   * Sets flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to set (bitmask)
   */
  public static void setFlags(String key, int flags) {
    getEntry(key).setFlags(flags);
  }

  /**
   * Clears flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to clear (bitmask)
   */
  public static void clearFlags(String key, int flags) {
    getEntry(key).clearFlags(flags);
  }

  /**
   * Returns the flags for the specified key.
   *
   * @param key the key name
   * @return the flags, or 0 if the key is not defined
   */
  public static int getFlags(String key) {
    return getEntry(key).getFlags();
  }

  /**
   * Deletes the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   */
  public static void delete(String key) {
    table.delete(key);
  }

  /**
   * Put a boolean in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putBoolean(String key, boolean value) {
    return getEntry(key).setBoolean(value);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key does not exist.
   * @return False if the table key exists with a different type
   */
  public static boolean setDefaultBoolean(String key, boolean defaultValue) {
    return getEntry(key).setDefaultBoolean(defaultValue);
  }

  /**
   * Returns the boolean the key maps to. If the key does not exist or is of
   *     different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public static boolean getBoolean(String key, boolean defaultValue) {
    return getEntry(key).getBoolean(defaultValue);
  }

  /**
   * Put a number in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putNumber(String key, double value) {
    return getEntry(key).setDouble(value);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key does not exist.
   * @return False if the table key exists with a different type
   */
  public static boolean setDefaultNumber(String key, double defaultValue) {
    return getEntry(key).setDefaultDouble(defaultValue);
  }

  /**
   * Returns the number the key maps to. If the key does not exist or is of
   *     different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public static double getNumber(String key, double defaultValue) {
    return getEntry(key).getDouble(defaultValue);
  }

  /**
   * Put a string in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putString(String key, String value) {
    return getEntry(key).setString(value);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key does not exist.
   * @return False if the table key exists with a different type
   */
  public static boolean setDefaultString(String key, String defaultValue) {
    return getEntry(key).setDefaultString(defaultValue);
  }

  /**
   * Returns the string the key maps to. If the key does not exist or is of
   *     different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public static String getString(String key, String defaultValue) {
    return getEntry(key).getString(defaultValue);
  }

  /**
   * Put a boolean array in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putBooleanArray(String key, boolean[] value) {
    return getEntry(key).setBooleanArray(value);
  }

  /**
   * Put a boolean array in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putBooleanArray(String key, Boolean[] value) {
    return getEntry(key).setBooleanArray(value);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key does not exist.
   * @return False if the table key exists with a different type
   */
  public static boolean setDefaultBooleanArray(String key, boolean[] defaultValue) {
    return getEntry(key).setDefaultBooleanArray(defaultValue);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key does not exist.
   * @return False if the table key exists with a different type
   */
  public static boolean setDefaultBooleanArray(String key, Boolean[] defaultValue) {
    return getEntry(key).setDefaultBooleanArray(defaultValue);
  }

  /**
   * Returns the boolean array the key maps to. If the key does not exist or is
   *     of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public static boolean[] getBooleanArray(String key, boolean[] defaultValue) {
    return getEntry(key).getBooleanArray(defaultValue);
  }

  /**
   * Returns the boolean array the key maps to. If the key does not exist or is
   *     of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public static Boolean[] getBooleanArray(String key, Boolean[] defaultValue) {
    return getEntry(key).getBooleanArray(defaultValue);
  }

  /**
   * Put a number array in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putNumberArray(String key, double[] value) {
    return getEntry(key).setDoubleArray(value);
  }

  /**
   * Put a number array in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putNumberArray(String key, Double[] value) {
    return getEntry(key).setNumberArray(value);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key does not exist.
   * @return False if the table key exists with a different type
   */
  public static boolean setDefaultNumberArray(String key, double[] defaultValue) {
    return getEntry(key).setDefaultDoubleArray(defaultValue);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key does not exist.
   * @return False if the table key exists with a different type
   */
  public static boolean setDefaultNumberArray(String key, Double[] defaultValue) {
    return getEntry(key).setDefaultNumberArray(defaultValue);
  }

  /**
   * Returns the number array the key maps to. If the key does not exist or is
   *     of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public static double[] getNumberArray(String key, double[] defaultValue) {
    return getEntry(key).getDoubleArray(defaultValue);
  }

  /**
   * Returns the number array the key maps to. If the key does not exist or is
   *     of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public static Double[] getNumberArray(String key, Double[] defaultValue) {
    return getEntry(key).getDoubleArray(defaultValue);
  }

  /**
   * Put a string array in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putStringArray(String key, String[] value) {
    return getEntry(key).setStringArray(value);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key does not exist.
   * @return False if the table key exists with a different type
   */
  public static boolean setDefaultStringArray(String key, String[] defaultValue) {
    return getEntry(key).setDefaultStringArray(defaultValue);
  }

  /**
   * Returns the string array the key maps to. If the key does not exist or is
   *     of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public static String[] getStringArray(String key, String[] defaultValue) {
    return getEntry(key).getStringArray(defaultValue);
  }

  /**
   * Put a raw value (byte array) in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putRaw(String key, byte[] value) {
    return getEntry(key).setRaw(value);
  }

  /**
   * Put a raw value (bytes from a byte buffer) in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @param len the length of the value
   * @return False if the table key already exists with a different type
   */
  public static boolean putRaw(String key, ByteBuffer value, int len) {
    return getEntry(key).setRaw(value, len);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key does not exist.
   * @return False if the table key exists with a different type
   */
  public static boolean setDefaultRaw(String key, byte[] defaultValue) {
    return getEntry(key).setDefaultRaw(defaultValue);
  }

  /**
   * Returns the raw value (byte array) the key maps to. If the key does not
   *     exist or is of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public static byte[] getRaw(String key, byte[] defaultValue) {
    return getEntry(key).getRaw(defaultValue);
  }

  /**
   * Puts all sendable data to the dashboard.
   */
  public static synchronized void updateValues() {
    for (Data data : tablesToData.values()) {
      data.m_builder.updateTable();
    }
  }
}
