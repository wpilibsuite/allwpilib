// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.networktables.EntryListenerFlags;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import java.util.Collection;

/**
 * The preferences class provides a relatively simple way to save important values to the roboRIO to
 * access the next time the roboRIO is booted.
 *
 * <p>This class loads and saves from a file inside the roboRIO. The user can not access the file
 * directly, but may modify values at specific fields which will then be automatically saved to the
 * file by the NetworkTables server.
 *
 * <p>This class is thread safe.
 *
 * <p>This will also interact with {@link NetworkTable} by creating a table called "Preferences"
 * with all the key-value pairs.
 */
public final class Preferences {
  /** The Preferences table name. */
  private static final String TABLE_NAME = "Preferences";
  /** The singleton instance. */
  private static Preferences instance;
  /** The network table. */
  private final NetworkTable m_table;

  /**
   * Returns the preferences instance.
   *
   * @return the preferences instance
   */
  public static synchronized Preferences getInstance() {
    if (instance == null) {
      instance = new Preferences();
    }
    return instance;
  }

  /** Creates a preference class. */
  private Preferences() {
    m_table = NetworkTableInstance.getDefault().getTable(TABLE_NAME);
    m_table.getEntry(".type").setString("RobotPreferences");
    // Listener to set all Preferences values to persistent
    // (for backwards compatibility with old dashboards).
    m_table.addEntryListener(
        (table, key, entry, value, flags) -> entry.setPersistent(),
        EntryListenerFlags.kImmediate | EntryListenerFlags.kNew);
    HAL.report(tResourceType.kResourceType_Preferences, 0);
  }

  /**
   * Gets the preferences keys.
   *
   * @return a collection of the keys
   */
  public Collection<String> getKeys() {
    return m_table.getKeys();
  }

  /**
   * Puts the given string into the preferences table.
   *
   * @param key the key
   * @param value the value
   * @throws NullPointerException if value is null
   */
  public void setString(String key, String value) {
    requireNonNullParam(value, "value", "setString");

    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setString(value);
    entry.setPersistent();
  }

  /**
   * Puts the given string into the preferences table.
   *
   * @param key the key
   * @param value the value
   * @throws NullPointerException if value is null
   * @deprecated Use {@link #setString(String, String)}
   */
  @Deprecated
  public void putString(String key, String value) {
    setString(key, value);
  }

  /**
   * Puts the given string into the preferences table if it doesn't already exist.
   *
   * @param key The key
   * @param value The value
   */
  public void initString(String key, String value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDefaultString(value);
  }

  /**
   * Puts the given int into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public void setInt(String key, int value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDouble(value);
    entry.setPersistent();
  }

  /**
   * Puts the given int into the preferences table.
   *
   * @param key the key
   * @param value the value
   * @deprecated Use {@link #setInt(String, int)}
   */
  @Deprecated
  public void putInt(String key, int value) {
    setInt(key, value);
  }

  /**
   * Puts the given int into the preferences table if it doesn't already exist.
   *
   * @param key The key
   * @param value The value
   */
  public void initInt(String key, int value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDefaultDouble(value);
  }

  /**
   * Puts the given double into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public void setDouble(String key, double value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDouble(value);
    entry.setPersistent();
  }

  /**
   * Puts the given double into the preferences table.
   *
   * @param key the key
   * @param value the value
   * @deprecated Use {@link #setDouble(String, double)}
   */
  public void putDouble(String key, double value) {
    setDouble(key, value);
  }

  /**
   * Puts the given double into the preferences table if it doesn't already exist.
   *
   * @param key The key
   * @param value The value
   */
  public void initDouble(String key, double value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDefaultDouble(value);
  }

  /**
   * Puts the given float into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public void setFloat(String key, float value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDouble(value);
    entry.setPersistent();
  }

  /**
   * Puts the given float into the preferences table.
   *
   * @param key the key
   * @param value the value
   * @deprecated Use {@link #setFloat(String, value)}
   */
  @Deprecated
  public void putFloat(String key, float value) {
    setFloat(key, value);
  }

  /**
   * Puts the given float into the preferences table if it doesn't already exist.
   *
   * @param key The key
   * @param value The value
   */
  public void initFloat(String key, float value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDefaultDouble(value);
  }

  /**
   * Puts the given boolean into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public void setBoolean(String key, boolean value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setBoolean(value);
    entry.setPersistent();
  }

  /**
   * Puts the given boolean into the preferences table.
   *
   * @param key the key
   * @param value the value
   * @deprecated Use {@link #setBoolean(String, value)}
   */
  @Deprecated
  public void putBoolean(String key, boolean value) {
    setBoolean(key, value);
  }

  /**
   * Puts the given boolean into the preferences table if it doesn't already exist.
   *
   * @param key The key
   * @param value The value
   */
  public void initBoolean(String key, boolean value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDefaultBoolean(value);
  }

  /**
   * Puts the given long into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public void setLong(String key, long value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDouble(value);
    entry.setPersistent();
  }

  /**
   * Puts the given long into the preferences table.
   *
   * @param key the key
   * @param value the value
   * @deprecated Use {@link #setLong(String, long)}
   */
  @Deprecated
  public void putLong(String key, long value) {
    setLong(key, value);
  }

  /**
   * Puts the given long into the preferences table if it doesn't already exist.
   *
   * @param key The key
   * @param value The value
   */
  public void initLong(String key, long value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDefaultDouble(value);
  }

  /**
   * Returns whether or not there is a key with the given name.
   *
   * @param key the key
   * @return if there is a value at the given key
   */
  public boolean containsKey(String key) {
    return m_table.containsKey(key);
  }

  /**
   * Remove a preference.
   *
   * @param key the key
   */
  public void remove(String key) {
    m_table.delete(key);
  }

  /** Remove all preferences. */
  public void removeAll() {
    for (String key : m_table.getKeys()) {
      if (!".type".equals(key)) {
        remove(key);
      }
    }
  }

  /**
   * Returns the string at the given key. If this table does not have a value for that position,
   * then the given backup value will be returned.
   *
   * @param key the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   */
  public String getString(String key, String backup) {
    return m_table.getEntry(key).getString(backup);
  }

  /**
   * Returns the int at the given key. If this table does not have a value for that position, then
   * the given backup value will be returned.
   *
   * @param key the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   */
  public int getInt(String key, int backup) {
    return (int) m_table.getEntry(key).getDouble(backup);
  }

  /**
   * Returns the double at the given key. If this table does not have a value for that position,
   * then the given backup value will be returned.
   *
   * @param key the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   */
  public double getDouble(String key, double backup) {
    return m_table.getEntry(key).getDouble(backup);
  }

  /**
   * Returns the boolean at the given key. If this table does not have a value for that position,
   * then the given backup value will be returned.
   *
   * @param key the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   */
  public boolean getBoolean(String key, boolean backup) {
    return m_table.getEntry(key).getBoolean(backup);
  }

  /**
   * Returns the float at the given key. If this table does not have a value for that position, then
   * the given backup value will be returned.
   *
   * @param key the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   */
  public float getFloat(String key, float backup) {
    return (float) m_table.getEntry(key).getDouble(backup);
  }

  /**
   * Returns the long at the given key. If this table does not have a value for that position, then
   * the given backup value will be returned.
   *
   * @param key the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   */
  public long getLong(String key, long backup) {
    return (long) m_table.getEntry(key).getDouble(backup);
  }
}
