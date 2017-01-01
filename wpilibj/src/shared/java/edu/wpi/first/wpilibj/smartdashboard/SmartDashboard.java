/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.smartdashboard;

import java.nio.ByteBuffer;
import java.util.Hashtable;
import java.util.NoSuchElementException;
import java.util.Set;

import edu.wpi.first.wpilibj.HLUsageReporting;
import edu.wpi.first.wpilibj.NamedSendable;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.networktables.NetworkTable;
import edu.wpi.first.wpilibj.networktables.NetworkTableKeyNotDefined;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.TableKeyNotDefinedException;

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
  private static final NetworkTable table = NetworkTable.getTable("SmartDashboard");
  /**
   * A table linking tables in the SmartDashboard to the {@link Sendable} objects they
   * came from.
   */
  private static final Hashtable tablesToData = new Hashtable();

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
  public static void putData(String key, Sendable data) {
    ITable dataTable = table.getSubTable(key);
    dataTable.putString("~TYPE~", data.getSmartDashboardType());
    data.initTable(dataTable);
    tablesToData.put(data, key);
  }


  // TODO should we reimplement NamedSendable?

  /**
   * Maps the specified key (where the key is the name of the {@link NamedSendable}
   * SmartDashboardNamedData to the specified value in this table. The value can be retrieved by
   * calling the get method with a key that is equal to the original key.
   *
   * @param value the value
   * @throws IllegalArgumentException If key is null
   */
  public static void putData(NamedSendable value) {
    putData(value.getName(), value);
  }

  /**
   * Returns the value at the specified key.
   *
   * @param key the key
   * @return the value
   * @throws NetworkTableKeyNotDefined if there is no value mapped to by the key
   * @throws IllegalArgumentException  if the key is null
   */
  public static Sendable getData(String key) {
    ITable subtable = table.getSubTable(key);
    Object data = tablesToData.get(subtable);
    if (data == null) {
      throw new IllegalArgumentException("SmartDashboard data does not exist: " + key);
    } else {
      return (Sendable) data;
    }
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
   * @param types bitmask of types; 0 is treated as a "don't care".
   * @return keys currently in the table
   */
  public Set<String> getKeys(int types) {
    return table.getKeys(types);
  }

  /**
   * @return keys currently in the table.
   */
  public Set<String> getKeys() {
    return table.getKeys();
  }

  /**
   * Makes a key's value persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  public void setPersistent(String key) {
    table.setPersistent(key);
  }

  /**
   * Stop making a key's value persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  public void clearPersistent(String key) {
    table.clearPersistent(key);
  }

  /**
   * Returns whether the value is persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   * @return True if the value is persistent.
   */
  public boolean isPersistent(String key) {
    return table.isPersistent(key);
  }

  /**
   * Sets flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to set (bitmask)
   */
  public void setFlags(String key, int flags) {
    table.setFlags(key, flags);
  }

  /**
   * Clears flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to clear (bitmask)
   */
  public void clearFlags(String key, int flags) {
    table.clearFlags(key, flags);
  }

  /**
   * Returns the flags for the specified key.
   *
   * @param key the key name
   * @return the flags, or 0 if the key is not defined
   */
  public int getFlags(String key) {
    return table.getFlags(key);
  }

  /**
   * Deletes the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   */
  public void delete(String key) {
    table.delete(key);
  }

  /**
   * Put a boolean in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putBoolean(String key, boolean value) {
    return table.putBoolean(key, value);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doens't exist.
   * @return False if the table key exists with a different type
   */
  public boolean setDefaultBoolean(String key, boolean defaultValue) {
    return table.setDefaultBoolean(key, defaultValue);
  }

  /**
   * Returns the boolean the key maps to.
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   *     the given key
   * @deprecated This exception-raising method has been replaced by the
   *     default-taking method {@link #getBoolean(String, boolean)}.
   */
  @Deprecated
  public static boolean getBoolean(String key) throws TableKeyNotDefinedException {
    return table.getBoolean(key);
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
    return table.getBoolean(key, defaultValue);
  }

  /**
   * Put a number in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putNumber(String key, double value) {
    return table.putNumber(key, value);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doens't exist.
   * @return False if the table key exists with a different type
   */
  public boolean setDefaultNumber(String key, double defaultValue) {
    return table.setDefaultNumber(key, defaultValue);
  }

  /**
   * Returns the number the key maps to.
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   *     the given key
   * @deprecated This exception-raising method has been replaced by the
   *     default-taking method {@link #getNumber(String, double)}.
   */
  @Deprecated
  public static double getNumber(String key) throws TableKeyNotDefinedException {
    return table.getNumber(key);
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
    return table.getNumber(key, defaultValue);
  }

  /**
   * Put a string in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public static boolean putString(String key, String value) {
    return table.putString(key, value);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doens't exist.
   * @return False if the table key exists with a different type
   */
  public boolean setDefaultString(String key, String defaultValue) {
    return table.setDefaultString(key, defaultValue);
  }

  /**
   * Returns the string the key maps to.
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   *     the given key
   * @deprecated This exception-raising method has been replaced by the
   *     default-taking method {@link #getString(String, String)}.
   */
  @Deprecated
  public static String getString(String key) throws TableKeyNotDefinedException {
    return table.getString(key);
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
    return table.getString(key, defaultValue);
  }

  /**
   * Put a boolean array in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putBooleanArray(String key, boolean[] value) {
    return table.putBooleanArray(key, value);
  }

  /**
   * Put a boolean array in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putBooleanArray(String key, Boolean[] value) {
    return table.putBooleanArray(key, value);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doens't exist.
   * @return False if the table key exists with a different type
   */
  public boolean setDefaultBooleanArray(String key, boolean[] defaultValue) {
    return table.setDefaultBooleanArray(key, defaultValue);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doens't exist.
   * @return False if the table key exists with a different type
   */
  public boolean setDefaultBooleanArray(String key, Boolean[] defaultValue) {
    return table.setDefaultBooleanArray(key, defaultValue);
  }

  /**
   * Returns the boolean array the key maps to.
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   *     the given key
   * @deprecated This exception-raising method has been replaced by the
   *     default-taking method {@link #getBooleanArray(String, boolean[])}.
   */
  @Deprecated
  public boolean[] getBooleanArray(String key) throws TableKeyNotDefinedException {
    return table.getBooleanArray(key);
  }

  /**
   * Returns the boolean array the key maps to. If the key does not exist or is
   *     of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public boolean[] getBooleanArray(String key, boolean[] defaultValue) {
    return table.getBooleanArray(key, defaultValue);
  }

  /**
   * Returns the boolean array the key maps to. If the key does not exist or is
   *     of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public Boolean[] getBooleanArray(String key, Boolean[] defaultValue) {
    return table.getBooleanArray(key, defaultValue);
  }

  /**
   * Put a number array in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putNumberArray(String key, double[] value) {
    return table.putNumberArray(key, value);
  }

  /**
   * Put a number array in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putNumberArray(String key, Double[] value) {
    return table.putNumberArray(key, value);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doens't exist.
   * @return False if the table key exists with a different type
   */
  public boolean setDefaultNumberArray(String key, double[] defaultValue) {
    return table.setDefaultNumberArray(key, defaultValue);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doens't exist.
   * @return False if the table key exists with a different type
   */
  public boolean setDefaultNumberArray(String key, Double[] defaultValue) {
    return table.setDefaultNumberArray(key, defaultValue);
  }

  /**
   * Returns the number array the key maps to.
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   *     the given key
   * @deprecated This exception-raising method has been replaced by the
   *     default-taking method {@link #getNumberArray(String, double[])}.
   */
  @Deprecated
  public double[] getNumberArray(String key) throws TableKeyNotDefinedException {
    return table.getNumberArray(key);
  }

  /**
   * Returns the number array the key maps to. If the key does not exist or is
   *     of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public double[] getNumberArray(String key, double[] defaultValue) {
    return table.getNumberArray(key, defaultValue);
  }

  /**
   * Returns the number array the key maps to. If the key does not exist or is
   *     of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public Double[] getNumberArray(String key, Double[] defaultValue) {
    return table.getNumberArray(key, defaultValue);
  }

  /**
   * Put a string array in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putStringArray(String key, String[] value) {
    return table.putStringArray(key, value);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doens't exist.
   * @return False if the table key exists with a different type
   */
  public boolean setDefaultStringArray(String key, String[] defaultValue) {
    return table.setDefaultStringArray(key, defaultValue);
  }

  /**
   * Returns the string array the key maps to.
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   *     the given key
   * @deprecated This exception-raising method has been replaced by the
   *     default-taking method {@link #getStringArray(String, String[])}.
   */
  @Deprecated
  public String[] getStringArray(String key) throws TableKeyNotDefinedException {
    return table.getStringArray(key);
  }

  /**
   * Returns the string array the key maps to. If the key does not exist or is
   *     of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public String[] getStringArray(String key, String[] defaultValue) {
    return table.getStringArray(key, defaultValue);
  }

  /**
   * Put a raw value (byte array) in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putRaw(String key, byte[] value) {
    return table.putRaw(key, value);
  }

  /**
   * Put a raw value (bytes from a byte buffer) in the table.
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @param len the length of the value
   * @return False if the table key already exists with a different type
   */
  public boolean putRaw(String key, ByteBuffer value, int len) {
    return table.putRaw(key, value, len);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doens't exist.
   * @return False if the table key exists with a different type
   */
  public boolean setDefaultRaw(String key, byte[] defaultValue) {
    return table.setDefaultRaw(key, defaultValue);
  }

  /**
   * Returns the raw value (byte array) the key maps to.
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   *     the given key
   * @deprecated This exception-raising method has been replaced by the
   *     default-taking method {@link #getRaw(String, byte[])}.
   */
  @Deprecated
  public byte[] getRaw(String key) throws TableKeyNotDefinedException {
    return table.getRaw(key);
  }

  /**
   * Returns the raw value (byte array) the key maps to. If the key does not
   *     exist or is of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   *     if there is no value associated with the key
   */
  public byte[] getRaw(String key, byte[] defaultValue) {
    return table.getRaw(key, defaultValue);
  }

  /*
   * Deprecated Methods
   */

  /**
   * Maps the specified key to the specified value in this table.
   *
   * <p>The key can not be null. The value can be retrieved by calling the get method with a key
   * that is equal to the original key.
   *
   * @param key   the key
   * @param value the value
   * @throws IllegalArgumentException if key is null
   * @deprecated Use {@link #putNumber(java.lang.String, double) putNumber method} instead
   */
  @Deprecated
  public static void putInt(String key, int value) {
    table.putNumber(key, value);
  }

  /**
   * Returns the value at the specified key.
   *
   * @param key the key
   * @return the value
   * @throws TableKeyNotDefinedException if there is no value mapped to by the key
   * @throws IllegalArgumentException    if the value mapped to by the key is not an int
   * @throws IllegalArgumentException    if the key is null
   * @deprecated Use {@link #getNumber(java.lang.String) getNumber} instead
   */
  @Deprecated
  public static int getInt(String key) throws TableKeyNotDefinedException {
    return (int) table.getNumber(key);
  }

  /**
   * Returns the value at the specified key.
   *
   * @param key          the key
   * @param defaultValue the value returned if the key is undefined
   * @return the value
   * @throws TableKeyNotDefinedException if there is no value mapped to by the key
   * @throws IllegalArgumentException    if the value mapped to by the key is not an int
   * @throws IllegalArgumentException    if the key is null
   * @deprecated Use {@link #getNumber(java.lang.String, double) getNumber} instead
   */
  @Deprecated
  public static int getInt(String key, int defaultValue) throws TableKeyNotDefinedException {
    try {
      return (int) table.getNumber(key);
    } catch (NoSuchElementException ex) {
      return defaultValue;
    }
  }

  /**
   * Maps the specified key to the specified value in this table.
   *
   * <p>The key can not be null. The value can be retrieved by calling the get method with a key
   * that is equal to the original key.
   *
   * @param key   the key
   * @param value the value
   * @throws IllegalArgumentException if key is null
   * @deprecated Use{@link #putNumber(java.lang.String, double) putNumber} instead
   */
  @Deprecated
  public static void putDouble(String key, double value) {
    table.putNumber(key, value);
  }

  /**
   * Returns the value at the specified key.
   *
   * @param key the key
   * @return the value
   * @throws TableKeyNotDefinedException if there is no value mapped to by the key
   * @throws IllegalArgumentException    if the value mapped to by the key is not a double
   * @throws IllegalArgumentException    if the key is null
   * @deprecated Use {@link #getNumber(java.lang.String) getNumber} instead
   */
  @Deprecated
  public static double getDouble(String key) throws TableKeyNotDefinedException {
    return table.getNumber(key);
  }

  /**
   * Returns the value at the specified key.
   *
   * @param key          the key
   * @param defaultValue the value returned if the key is undefined
   * @return the value
   * @throws IllegalArgumentException if the value mapped to by the key is not a double
   * @throws IllegalArgumentException if the key is null
   * @deprecated Use {@link #getNumber(java.lang.String, double) getNumber} instead.
   */
  @Deprecated
  public static double getDouble(String key, double defaultValue) {
    return table.getNumber(key, defaultValue);
  }

}
