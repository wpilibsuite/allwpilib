// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableRegistry;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/**
 * The {@link SmartDashboard} class is the bridge between robot programs and the SmartDashboard on
 * the laptop.
 *
 * <p>When a value is put into the SmartDashboard here, it pops up on the SmartDashboard on the
 * laptop. Users can put values into and get values from the SmartDashboard.
 */
public final class SmartDashboard {
  /** The {@link NetworkTable} used by {@link SmartDashboard}. */
  private static NetworkTable table;

  /**
   * A table linking tables in the SmartDashboard to the {@link Sendable} objects they came from.
   */
  @SuppressWarnings("PMD.UseConcurrentHashMap")
  private static final Map<String, Sendable> tablesToData = new HashMap<>();

  /** The executor for listener tasks; calls listener tasks synchronously from main thread. */
  private static final ListenerExecutor listenerExecutor = new ListenerExecutor();

  private static boolean m_reported = false; // NOPMD redundant field initializer

  static {
    setNetworkTableInstance(NetworkTableInstance.getDefault());
  }

  private SmartDashboard() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Set the NetworkTable instance used for entries. For testing purposes; use with caution.
   *
   * @param inst NetworkTable instance
   */
  public static synchronized void setNetworkTableInstance(NetworkTableInstance inst) {
    SmartDashboard.table = inst.getTable("SmartDashboard");
    tablesToData.clear();
  }

  /**
   * Maps the specified key to the specified value in this table. The key can not be null. The value
   * can be retrieved by calling the get method with a key that is equal to the original key.
   *
   * @param key the key
   * @param data the value
   * @throws IllegalArgumentException If key is null
   */
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  public static synchronized void putData(String key, Sendable data) {
    if (!m_reported) {
      HAL.report(tResourceType.kResourceType_SmartDashboard, tInstances.kSmartDashboard_Instance);
      m_reported = true;
    }
    Sendable sddata = tablesToData.get(key);
    if (sddata == null || sddata != data) {
      tablesToData.put(key, data);
      NetworkTable dataTable = table.getSubTable(key);
      SendableBuilderImpl builder = new SendableBuilderImpl();
      builder.setTable(dataTable);
      SendableRegistry.publish(data, builder);
      builder.startListeners();
      dataTable.getEntry(".name").setString(key);
    }
  }

  /**
   * Maps the specified key (where the key is the name of the {@link Sendable}) to the specified
   * value in this table. The value can be retrieved by calling the get method with a key that is
   * equal to the original key.
   *
   * @param value the value
   * @throws IllegalArgumentException If key is null
   */
  public static void putData(Sendable value) {
    String name = SendableRegistry.getName(value);
    if (!name.isEmpty()) {
      putData(name, value);
    }
  }

  /**
   * Returns the value at the specified key.
   *
   * @param key the key
   * @return the value
   * @throws IllegalArgumentException if the key is null
   */
  public static synchronized Sendable getData(String key) {
    Sendable data = tablesToData.get(key);
    if (data == null) {
      throw new IllegalArgumentException("SmartDashboard data does not exist: " + key);
    } else {
      return data;
    }
  }

  /**
   * Gets the entry for the specified key.
   *
   * @param key the key name
   * @return Network table entry.
   */
  public static NetworkTableEntry getEntry(String key) {
    if (!m_reported) {
      HAL.report(tResourceType.kResourceType_SmartDashboard, tInstances.kSmartDashboard_Instance);
      m_reported = true;
    }
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
   * Makes a key's value persistent through program restarts. The key cannot be null.
   *
   * @param key the key name
   */
  public static void setPersistent(String key) {
    getEntry(key).setPersistent();
  }

  /**
   * Stop making a key's value persistent through program restarts. The key cannot be null.
   *
   * @param key the key name
   */
  public static void clearPersistent(String key) {
    getEntry(key).clearPersistent();
  }

  /**
   * Returns whether the value is persistent through program restarts. The key cannot be null.
   *
   * @param key the key name
   * @return True if the value is persistent.
   */
  public static boolean isPersistent(String key) {
    return getEntry(key).isPersistent();
  }

  /**
   * Put a boolean in the table.
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putBoolean(String key, boolean value) {
    return getEntry(key).setBoolean(value);
  }

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key the key
   * @param defaultValue the value to set if key does not exist
   * @return True if the key did not already exist, otherwise False
   */
  public static boolean setDefaultBoolean(String key, boolean defaultValue) {
    return getEntry(key).setDefaultBoolean(defaultValue);
  }

  /**
   * Returns the boolean the key maps to. If the key does not exist or is of different type, it will
   * return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value if there is no value
   *     associated with the key
   */
  public static boolean getBoolean(String key, boolean defaultValue) {
    return getEntry(key).getBoolean(defaultValue);
  }

  /**
   * Put a number in the table.
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putNumber(String key, double value) {
    return getEntry(key).setDouble(value);
  }

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key the key
   * @param defaultValue the value to set if key does not exist
   * @return True if the key did not already exist, otherwise False
   */
  public static boolean setDefaultNumber(String key, double defaultValue) {
    return getEntry(key).setDefaultDouble(defaultValue);
  }

  /**
   * Returns the number the key maps to. If the key does not exist or is of different type, it will
   * return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value if there is no value
   *     associated with the key
   */
  public static double getNumber(String key, double defaultValue) {
    return getEntry(key).getDouble(defaultValue);
  }

  /**
   * Put a string in the table.
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putString(String key, String value) {
    return getEntry(key).setString(value);
  }

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key the key
   * @param defaultValue the value to set if key does not exist
   * @return True if the key did not already exist, otherwise False
   */
  public static boolean setDefaultString(String key, String defaultValue) {
    return getEntry(key).setDefaultString(defaultValue);
  }

  /**
   * Returns the string the key maps to. If the key does not exist or is of different type, it will
   * return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value if there is no value
   *     associated with the key
   */
  public static String getString(String key, String defaultValue) {
    return getEntry(key).getString(defaultValue);
  }

  /**
   * Put a boolean array in the table.
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putBooleanArray(String key, boolean[] value) {
    return getEntry(key).setBooleanArray(value);
  }

  /**
   * Put a boolean array in the table.
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putBooleanArray(String key, Boolean[] value) {
    return getEntry(key).setBooleanArray(value);
  }

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key the key
   * @param defaultValue the value to set if key does not exist
   * @return True if the key did not already exist, otherwise False
   */
  public static boolean setDefaultBooleanArray(String key, boolean[] defaultValue) {
    return getEntry(key).setDefaultBooleanArray(defaultValue);
  }

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key the key
   * @param defaultValue the value to set if key does not exist
   * @return True if the key did not already exist, otherwise False
   */
  public static boolean setDefaultBooleanArray(String key, Boolean[] defaultValue) {
    return getEntry(key).setDefaultBooleanArray(defaultValue);
  }

  /**
   * Returns the boolean array the key maps to. If the key does not exist or is of different type,
   * it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value if there is no value
   *     associated with the key
   */
  public static boolean[] getBooleanArray(String key, boolean[] defaultValue) {
    return getEntry(key).getBooleanArray(defaultValue);
  }

  /**
   * Returns the boolean array the key maps to. If the key does not exist or is of different type,
   * it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value if there is no value
   *     associated with the key
   */
  public static Boolean[] getBooleanArray(String key, Boolean[] defaultValue) {
    return getEntry(key).getBooleanArray(defaultValue);
  }

  /**
   * Put a number array in the table.
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putNumberArray(String key, double[] value) {
    return getEntry(key).setDoubleArray(value);
  }

  /**
   * Put a number array in the table.
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putNumberArray(String key, Double[] value) {
    return getEntry(key).setNumberArray(value);
  }

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key the key
   * @param defaultValue the value to set if key does not exist
   * @return True if the key did not already exist, otherwise False
   */
  public static boolean setDefaultNumberArray(String key, double[] defaultValue) {
    return getEntry(key).setDefaultDoubleArray(defaultValue);
  }

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key the key
   * @param defaultValue the value to set if key does not exist
   * @return True if the key did not already exist, otherwise False
   */
  public static boolean setDefaultNumberArray(String key, Double[] defaultValue) {
    return getEntry(key).setDefaultNumberArray(defaultValue);
  }

  /**
   * Returns the number array the key maps to. If the key does not exist or is of different type, it
   * will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value if there is no value
   *     associated with the key
   */
  public static double[] getNumberArray(String key, double[] defaultValue) {
    return getEntry(key).getDoubleArray(defaultValue);
  }

  /**
   * Returns the number array the key maps to. If the key does not exist or is of different type, it
   * will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value if there is no value
   *     associated with the key
   */
  public static Double[] getNumberArray(String key, Double[] defaultValue) {
    return getEntry(key).getDoubleArray(defaultValue);
  }

  /**
   * Put a string array in the table.
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putStringArray(String key, String[] value) {
    return getEntry(key).setStringArray(value);
  }

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key the key
   * @param defaultValue the value to set if key does not exist
   * @return True if the key did not already exist, otherwise False
   */
  public static boolean setDefaultStringArray(String key, String[] defaultValue) {
    return getEntry(key).setDefaultStringArray(defaultValue);
  }

  /**
   * Returns the string array the key maps to. If the key does not exist or is of different type, it
   * will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value if there is no value
   *     associated with the key
   */
  public static String[] getStringArray(String key, String[] defaultValue) {
    return getEntry(key).getStringArray(defaultValue);
  }

  /**
   * Put a raw value (byte array) in the table.
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putRaw(String key, byte[] value) {
    return getEntry(key).setRaw(value);
  }

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key the key
   * @param defaultValue the value to set if key does not exist
   * @return True if the key did not already exist, otherwise False
   */
  public static boolean setDefaultRaw(String key, byte[] defaultValue) {
    return getEntry(key).setDefaultRaw(defaultValue);
  }

  /**
   * Returns the raw value (byte array) the key maps to. If the key does not exist or is of
   * different type, it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value if there is no value
   *     associated with the key
   */
  public static byte[] getRaw(String key, byte[] defaultValue) {
    return getEntry(key).getRaw(defaultValue);
  }

  /**
   * Posts a task from a listener to the ListenerExecutor, so that it can be run synchronously from
   * the main loop on the next call to {@link SmartDashboard#updateValues()}.
   *
   * @param task The task to run synchronously from the main thread.
   */
  public static void postListenerTask(Runnable task) {
    listenerExecutor.execute(task);
  }

  /** Puts all sendable data to the dashboard. */
  public static synchronized void updateValues() {
    // Execute posted listener tasks
    listenerExecutor.runListenerTasks();
    for (Sendable data : tablesToData.values()) {
      SendableRegistry.update(data);
    }
  }
}
