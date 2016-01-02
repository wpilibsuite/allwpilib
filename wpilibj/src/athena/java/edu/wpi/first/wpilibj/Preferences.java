/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.Vector;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.networktables.NetworkTable;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;
import edu.wpi.first.wpilibj.tables.TableKeyNotDefinedException;

/**
 * The preferences class provides a relatively simple way to save important
 * values to the RoboRIO to access the next time the RoboRIO is booted.
 *
 * <p>
 * This class loads and saves from a file inside the RoboRIO. The user can not
 * access the file directly, but may modify values at specific fields which will
 * then be automatically saved to the file by the NetworkTables server.
 * </p>
 *
 * <p>
 * This class is thread safe.
 * </p>
 *
 * <p>
 * This will also interact with {@link NetworkTable} by creating a table called
 * "Preferences" with all the key-value pairs.
 * </p>
 *
 * @author Joe Grinstead
 */
public class Preferences {

  /**
   * The Preferences table name
   */
  private static final String TABLE_NAME = "Preferences";
  /**
   * The singleton instance
   */
  private static Preferences instance;
  /**
   * The network table
   */
  private NetworkTable table;
  /**
   * Listener to set all Preferences values to persistent (for backwards
   * compatibility with old dashboards).
   */
  private final ITableListener listener = new ITableListener() {
    @Override
    public void valueChanged(ITable table, String key, Object value, boolean isNew) {
      // unused
    }
    @Override
    public void valueChangedEx(ITable table, String key, Object value, int flags) {
      table.setPersistent(key);
    }
  };

  /**
   * Returns the preferences instance.
   *
   * @return the preferences instance
   */
  public synchronized static Preferences getInstance() {
    if (instance == null) {
      instance = new Preferences();
    }
    return instance;
  }

  /**
   * Creates a preference class.
   */
  private Preferences() {
    table = NetworkTable.getTable(TABLE_NAME);
    table.addTableListenerEx(listener, ITable.NOTIFY_NEW | ITable.NOTIFY_IMMEDIATE);
    UsageReporting.report(tResourceType.kResourceType_Preferences, 0);
  }

  /**
   * @return a vector of the keys
   */
  public Vector getKeys() {
    Vector<String> keys = new Vector<String>();
    for (String key : table.getKeys()) {
      keys.add(key);
    }
    return keys;
  }

  /**
   * Puts the given string into the preferences table.
   *
   * @param key the key
   * @param value the value
   * @throws NullPointerException if value is null
   */
  public void putString(String key, String value) {
    if (value == null) {
      throw new NullPointerException();
    }
    table.putString(key, value);
    table.setPersistent(key);
  }

  /**
   * Puts the given int into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public void putInt(String key, int value) {
    table.putNumber(key, value);
    table.setPersistent(key);
  }

  /**
   * Puts the given double into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public void putDouble(String key, double value) {
    table.putNumber(key, value);
    table.setPersistent(key);
  }

  /**
   * Puts the given float into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public void putFloat(String key, float value) {
    table.putNumber(key, value);
    table.setPersistent(key);
  }

  /**
   * Puts the given boolean into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public void putBoolean(String key, boolean value) {
    table.putBoolean(key, value);
    table.setPersistent(key);
  }

  /**
   * Puts the given long into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public void putLong(String key, long value) {
    table.putNumber(key, value);
    table.setPersistent(key);
  }

  /**
   * Returns whether or not there is a key with the given name.
   *
   * @param key the key
   * @return if there is a value at the given key
   */
  public boolean containsKey(String key) {
    return table.containsKey(key);
  }

  /**
   * Remove a preference
   *
   * @param key the key
   */
  public void remove(String key) {
    table.delete(key);
  }

  /**
   * Returns the string at the given key. If this table does not have a value
   * for that position, then the given backup value will be returned.
   *
   * @param key the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   */
  public String getString(String key, String backup) {
    return table.getString(key, backup);
  }

  /**
   * Returns the int at the given key. If this table does not have a value for
   * that position, then the given backup value will be returned.
   *
   * @param key the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   */
  public int getInt(String key, int backup) {
    try {
      return (int)table.getNumber(key);
    } catch (TableKeyNotDefinedException e) {
      return backup;
    }
  }

  /**
   * Returns the double at the given key. If this table does not have a value
   * for that position, then the given backup value will be returned.
   *
   * @param key the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   */
  public double getDouble(String key, double backup) {
    return table.getDouble(key, backup);
  }

  /**
   * Returns the boolean at the given key. If this table does not have a value
   * for that position, then the given backup value will be returned.
   *
   * @param key the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   */
  public boolean getBoolean(String key, boolean backup) {
    return table.getBoolean(key, backup);
  }

  /**
   * Returns the float at the given key. If this table does not have a value for
   * that position, then the given backup value will be returned.
   *
   * @param key the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   */
  public float getFloat(String key, float backup) {
    try {
      return (float)table.getNumber(key);
    } catch (TableKeyNotDefinedException e) {
      return backup;
    }
  }

  /**
   * Returns the long at the given key. If this table does not have a value for
   * that position, then the given backup value will be returned.
   *
   * @param key the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   */
  public long getLong(String key, long backup) {
    try {
      return (long)table.getNumber(key);
    } catch (TableKeyNotDefinedException e) {
      return backup;
    }
  }

  /**
   * This function is no longer required, as NetworkTables automatically
   * saves persistent values (which all Preferences values are) periodically
   * when running as a server.
   * @deprecated backwards compatibility shim
   */
  public void save() {
  }
}
