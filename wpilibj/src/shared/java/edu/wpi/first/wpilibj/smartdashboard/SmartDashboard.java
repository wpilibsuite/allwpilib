/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.wpilibj.NamedSendable;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.networktables.NetworkTable;
import edu.wpi.first.wpilibj.networktables.NetworkTableKeyNotDefined;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.TableKeyNotDefinedException;
import edu.wpi.first.wpilibj.HLUsageReporting;
import java.util.Hashtable;
import java.util.NoSuchElementException;

/**
 * The {@link SmartDashboard} class is the bridge between robot programs and the
 * SmartDashboard on the laptop.
 *
 * <p>
 * When a value is put into the SmartDashboard here, it pops up on the
 * SmartDashboard on the laptop. Users can put values into and get values from
 * the SmartDashboard
 * </p>
 *
 * @author Joe Grinstead
 */
public class SmartDashboard {
  /** The {@link NetworkTable} used by {@link SmartDashboard} */
  private static final NetworkTable table = NetworkTable.getTable("SmartDashboard");
  /**
   * A table linking tables in the SmartDashboard to the
   * {@link SmartDashboardData} objects they came from.
   */
  private static final Hashtable tablesToData = new Hashtable();

  static {
    HLUsageReporting.reportSmartDashboard();
  }

  /**
   * Maps the specified key to the specified value in this table. The key can
   * not be null. The value can be retrieved by calling the get method with a
   * key that is equal to the original key.
   *$
   * @param key the key
   * @param data the value
   * @throws IllegalArgumentException if key is null
   */
  public static void putData(String key, Sendable data) {
    ITable dataTable = table.getSubTable(key);
    dataTable.putString("~TYPE~", data.getSmartDashboardType());
    data.initTable(dataTable);
    tablesToData.put(data, key);
  }


  // TODO should we reimplement NamedSendable?
  /**
   * Maps the specified key (where the key is the name of the
   * {@link NamedSendable} SmartDashboardNamedData to the specified value in
   * this table. The value can be retrieved by calling the get method with a key
   * that is equal to the original key.
   *$
   * @param value the value
   * @throws IllegalArgumentException if key is null
   */
  public static void putData(NamedSendable value) {
    putData(value.getName(), value);
  }

  /**
   * Returns the value at the specified key.
   *$
   * @param key the key
   * @return the value
   * @throws NetworkTableKeyNotDefined if there is no value mapped to by the key
   * @throws IllegalArgumentException if the key is null
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
   * Maps the specified key to the specified value in this table. The key can
   * not be null. The value can be retrieved by calling the get method with a
   * key that is equal to the original key.
   *$
   * @param key the key
   * @param value the value
   * @throws IllegalArgumentException if key is null
   */
  public static void putBoolean(String key, boolean value) {
    table.putBoolean(key, value);
  }

  /**
   * Returns the value at the specified key.
   *$
   * @param key the key
   * @return the value
   * @throws NetworkTableKeyNotDefined if there is no value mapped to by the key
   * @throws IllegalArgumentException if the value mapped to by the key is not a
   *         boolean
   * @throws IllegalArgumentException if the key is null
   */
  public static boolean getBoolean(String key) throws TableKeyNotDefinedException {
    return table.getBoolean(key);
  }

  /**
   * Returns the value at the specified key.
   *$
   * @param key the key
   * @param defaultValue returned if the key doesn't exist
   * @return the value
   * @throws IllegalArgumentException if the value mapped to by the key is not a
   *         boolean
   * @throws IllegalArgumentException if the key is null
   */
  public static boolean getBoolean(String key, boolean defaultValue) {
    return table.getBoolean(key, defaultValue);
  }

  /**
   * Maps the specified key to the specified value in this table. The key can
   * not be null. The value can be retrieved by calling the get method with a
   * key that is equal to the original key.
   *$
   * @param key the key
   * @param value the value
   * @throws IllegalArgumentException if key is null
   */
  public static void putNumber(String key, double value) {
    table.putNumber(key, value);
  }

  /**
   * Returns the value at the specified key.
   *$
   * @param key the key
   * @return the value
   * @throws TableKeyNotDefinedException if there is no value mapped to by the
   *         key
   * @throws IllegalArgumentException if the value mapped to by the key is not a
   *         double
   * @throws IllegalArgumentException if the key is null
   */
  public static double getNumber(String key) throws TableKeyNotDefinedException {
    return table.getNumber(key);
  }

  /**
   * Returns the value at the specified key.
   *$
   * @param key the key
   * @param defaultValue the value returned if the key is undefined
   * @return the value
   * @throws NetworkTableKeyNotDefined if there is no value mapped to by the key
   * @throws IllegalArgumentException if the value mapped to by the key is not a
   *         double
   * @throws IllegalArgumentException if the key is null
   */
  public static double getNumber(String key, double defaultValue) {
    return table.getNumber(key, defaultValue);
  }

  /**
   * Maps the specified key to the specified value in this table. Neither the
   * key nor the value can be null. The value can be retrieved by calling the
   * get method with a key that is equal to the original key.
   *$
   * @param key the key
   * @param value the value
   * @throws IllegalArgumentException if key or value is null
   */
  public static void putString(String key, String value) {
    table.putString(key, value);
  }

  /**
   * Returns the value at the specified key.
   *$
   * @param key the key
   * @return the value
   * @throws NetworkTableKeyNotDefined if there is no value mapped to by the key
   * @throws IllegalArgumentException if the value mapped to by the key is not a
   *         string
   * @throws IllegalArgumentException if the key is null
   */
  public static String getString(String key) throws TableKeyNotDefinedException {
    return table.getString(key);
  }

  /**
   * Returns the value at the specified key.
   *$
   * @param key the key
   * @param defaultValue The value returned if the key is undefined
   * @return the value
   * @throws NetworkTableKeyNotDefined if there is no value mapped to by the key
   * @throws IllegalArgumentException if the value mapped to by the key is not a
   *         string
   * @throws IllegalArgumentException if the key is null
   */
  public static String getString(String key, String defaultValue) {
    return table.getString(key, defaultValue);
  }



  /*
   * Deprecated Methods
   */
  /**
   * Maps the specified key to the specified value in this table.
   *
   * The key can not be null. The value can be retrieved by calling the get
   * method with a key that is equal to the original key.
   *
   * @deprecated Use {@link #putNumber(java.lang.String, double) putNumber
   *             method} instead
   * @param key the key
   * @param value the value
   * @throws IllegalArgumentException if key is null
   */
  public static void putInt(String key, int value) {
    table.putNumber(key, value);
  }

  /**
   * Returns the value at the specified key.
   *
   * @deprecated Use {@link #getNumber(java.lang.String) getNumber} instead
   * @param key the key
   * @return the value
   * @throws TableKeyNotDefinedException if there is no value mapped to by the
   *         key
   * @throws IllegalArgumentException if the value mapped to by the key is not
   *         an int
   * @throws IllegalArgumentException if the key is null
   */
  public static int getInt(String key) throws TableKeyNotDefinedException {
    return (int) table.getNumber(key);
  }

  /**
   * Returns the value at the specified key.
   *
   * @deprecated Use {@link #getNumber(java.lang.String, double) getNumber}
   *             instead
   * @param key the key
   * @param defaultValue the value returned if the key is undefined
   * @return the value
   * @throws TableKeyNotDefinedException if there is no value mapped to by the
   *         key
   * @throws IllegalArgumentException if the value mapped to by the key is not
   *         an int
   * @throws IllegalArgumentException if the key is null
   */
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
   * The key can not be null. The value can be retrieved by calling the get
   * method with a key that is equal to the original key.
   *
   * @deprecated Use{@link #putNumber(java.lang.String, double) putNumber}
   *             instead
   * @param key the key
   * @param value the value
   * @throws IllegalArgumentException if key is null
   */
  public static void putDouble(String key, double value) {
    table.putNumber(key, value);
  }

  /**
   * Returns the value at the specified key.
   *
   * @deprecated Use {@link #getNumber(java.lang.String) getNumber} instead
   * @param key the key
   * @return the value
   * @throws TableKeyNotDefinedException if there is no value mapped to by the
   *         key
   * @throws IllegalArgumentException if the value mapped to by the key is not a
   *         double
   * @throws IllegalArgumentException if the key is null
   */
  public static double getDouble(String key) throws TableKeyNotDefinedException {
    return table.getNumber(key);
  }

  /**
   * Returns the value at the specified key.
   *
   * @deprecated Use {@link #getNumber(java.lang.String, double) getNumber}
   *             instead.
   * @param key the key
   * @param defaultValue the value returned if the key is undefined
   * @return the value
   * @throws TableKeyNotDefinedException if there is no value mapped to by the
   *         key
   * @throws IllegalArgumentException if the value mapped to by the key is not a
   *         double
   * @throws IllegalArgumentException if the key is null
   */
  public static double getDouble(String key, double defaultValue) {
    return table.getNumber(key, defaultValue);
  }

}
