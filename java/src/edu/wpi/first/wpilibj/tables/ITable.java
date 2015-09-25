package edu.wpi.first.wpilibj.tables;

import java.util.NoSuchElementException;
import java.util.Set;


/**
 * A table whose values can be read and written to
 * 
 * @author Mitchell
 * 
 */
public interface ITable {

  /**
   * @param key the key to search for
   * @return true if the table as a value assigned to the given key
   */
  public boolean containsKey(String key);

  /**
   * @param key the key to search for
   * @return true if there is a subtable with the key which contains at least
   * one key/subtable of its own
   */
  public boolean containsSubTable(String key);

  /**
   * @param key the name of the table relative to this one
   * @return a sub table relative to this one
   */
  public ITable getSubTable(String key);

  /**
   * @param types bitmask of types; 0 is treated as a "don't care".
   * @return keys currently in the table
   */
  public Set<String> getKeys(int types);

  /**
   * @return keys currently in the table
   */
  public Set<String> getKeys();

  /**
   * @return subtables currently in the table
   */
  public Set<String> getSubTables();

  /**
   * Makes a key's value persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  public void setPersistent(String key);

  /**
   * Stop making a key's value persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  public void clearPersistent(String key);

  /**
   * Returns whether the value is persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   * @return True if the value is persistent.
   */
  public boolean isPersistent(String key);

  /**
   * Sets flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to set (bitmask)
   */
  public void setFlags(String key, int flags);

  /**
   * Clears flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to clear (bitmask)
   */
  public void clearFlags(String key, int flags);

  /**
   * Returns the flags for the specified key.
   *
   * @param key
   *            the key name
   * @return the flags, or 0 if the key is not defined
   */
  public int getFlags(String key);

  /**
   * Deletes the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   */
  public void delete(String key);

  /**
   * Gets the value associated with a key as an object
   * @param key the key of the value to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   * the given key
   */
  public Object getValue(String key) throws TableKeyNotDefinedException;

  /**
   * Put a value in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   * @throws IllegalArgumentException when the value is not supported by the
   * table
   */
  public boolean putValue(String key, Object value)
      throws IllegalArgumentException;

  /**
   * Retrieve an array data type from the table.
   * @param key the key to be assigned to
   * @param externalValue the array data type to retreive into
   * @throws TableKeyNotDefinedException if there is no value associated with
   * the given key
   * @deprecated Use get*Array functions instead.
   */
  public void retrieveValue(String key, Object externalValue) throws TableKeyNotDefinedException;

  /**
   * Put a number in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putNumber(String key, double value);
  /**
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   * the given key
   */
  public double getNumber(String key) throws TableKeyNotDefinedException;
  /**
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  public double getNumber(String key, double defaultValue);

  /**
   * Put a string in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putString(String key, String value);
  /**
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   * the given key
   */
  public String getString(String key) throws TableKeyNotDefinedException;
  /**
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  public String getString(String key, String defaultValue);

  /**
   * Put a boolean in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putBoolean(String key, boolean value);
  /**
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   * the given key
   */
  public boolean getBoolean(String key) throws TableKeyNotDefinedException;
  /**
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  public boolean getBoolean(String key, boolean defaultValue);

  /**
   * Put a boolean array in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putBooleanArray(String key, boolean[] value);
  /**
   * Put a boolean array in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putBooleanArray(String key, Boolean[] value);
  /**
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   * the given key
   */
  public boolean[] getBooleanArray(String key) throws TableKeyNotDefinedException;
  /**
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  public boolean[] getBooleanArray(String key, boolean[] defaultValue);
  /**
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  public Boolean[] getBooleanArray(String key, Boolean[] defaultValue);

  /**
   * Put a number array in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putNumberArray(String key, double[] value);
  /**
   * Put a number array in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putNumberArray(String key, Double[] value);
  /**
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   * the given key
   */
  public double[] getNumberArray(String key) throws TableKeyNotDefinedException;
  /**
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  public double[] getNumberArray(String key, double[] defaultValue);
  /**
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  public Double[] getNumberArray(String key, Double[] defaultValue);

  /**
   * Put a string array in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putStringArray(String key, String[] value);
  /**
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   * the given key
   */
  public String[] getStringArray(String key) throws TableKeyNotDefinedException;
  /**
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  public String[] getStringArray(String key, String[] defaultValue);

  /** Notifier flag values. */
  public static final int NOTIFY_IMMEDIATE = 0x01;
  public static final int NOTIFY_LOCAL = 0x02;
  public static final int NOTIFY_NEW = 0x04;
  public static final int NOTIFY_DELETE = 0x08;
  public static final int NOTIFY_UPDATE = 0x10;
  public static final int NOTIFY_FLAGS = 0x20;

  /**
   * Add a listener for changes to the table
   * @param listener the listener to add
   */
  public void addTableListener(ITableListener listener);
  /**
   * Add a listener for changes to the table
   * @param listener the listener to add
   * @param immediateNotify if true then this listener will be notified of all
   * current entries (marked as new)
   */
  public void addTableListener(ITableListener listener,
                               boolean immediateNotify);
  /**
   * Add a listener for changes to the table
   * @param listener the listener to add
   * @param flags bitmask specifying desired notifications
   */
  public void addTableListenerEx(ITableListener listener, int flags);

  /**
   * Add a listener for changes to a specific key the table
   * @param key the key to listen for
   * @param listener the listener to add
   * @param immediateNotify if true then this listener will be notified of all
   * current entries (marked as new)
   */
  public void addTableListener(String key, ITableListener listener,
                               boolean immediateNotify);
  /**
   * Add a listener for changes to a specific key the table
   * @param key the key to listen for
   * @param listener the listener to add
   * @param flags bitmask specifying desired notifications
   */
  public void addTableListenerEx(String key, ITableListener listener,
                                 int flags);
  /**
   * This will immediately notify the listener of all current sub tables
   * @param listener
   */
  public void addSubTableListener(final ITableListener listener);
  /**
   * This will immediately notify the listener of all current sub tables
   * @param listener
   * @param localNotify if true then this listener will be notified of all
   * local changes in addition to all remote changes
   */
  public void addSubTableListener(final ITableListener listener,
                                  boolean localNotify);
  /**
   * Remove a listener from receiving table events
   * @param listener the listener to be removed
   */
  public void removeTableListener(ITableListener listener);

  /*
   * Deprecated Methods
   */

  /**
   * @deprecated
   * Maps the specified key to the specified value in this table.
   * The key can not be null.
   * The value can be retrieved by calling the get method with a key that is
   * equal to the original key.
   * @param key the key
   * @param value the value
   * @return False if the table key already exists with a different type
   * @throws IllegalArgumentException if key is null
   */
  public boolean putInt(String key, int value);

  /**
   * @deprecated
   * Returns the value at the specified key.
   * @param key the key
   * @return the value
   * @throws TableKeyNotDefinedException if there is no value mapped to by the
   * key
   * @throws IllegalArgumentException if the value mapped to by the key is not
   * an int
   * @throws IllegalArgumentException if the key is null
   */
  public int getInt(String key) throws TableKeyNotDefinedException;

  /**
   * @deprecated
   * Returns the value at the specified key.
   * @param key the key
   * @param defaultValue the value returned if the key is undefined
   * @return the value
   * @throws NetworkTableKeyNotDefined if there is no value mapped to by the key
   * @throws IllegalArgumentException if the value mapped to by the key is not
   * an int
   * @throws IllegalArgumentException if the key is null
   */
  public int getInt(String key, int defaultValue)
      throws TableKeyNotDefinedException;

  /**
   * @deprecated
   * Maps the specified key to the specified value in this table.
   * The key can not be null.
   * The value can be retrieved by calling the get method with a key that is
   * equal to the original key.
   * @param key the key
   * @param value the value
   * @return False if the table key already exists with a different type
   * @throws IllegalArgumentException if key is null
   */
  public boolean putDouble(String key, double value);

  /**
   * @deprecated
   * Returns the value at the specified key.
   * @param key the key
   * @return the value
   * @throws NoSuchEleNetworkTableKeyNotDefinedmentException if there is no
   * value mapped to by the key
   * @throws IllegalArgumentException if the value mapped to by the key is not a
   * double
   * @throws IllegalArgumentException if the key is null
   */
  public double getDouble(String key) throws TableKeyNotDefinedException;

  /**
   * @deprecated
   * Returns the value at the specified key.
   * @param key the key
   * @param defaultValue the value returned if the key is undefined
   * @return the value
   * @throws NoSuchEleNetworkTableKeyNotDefinedmentException if there is no
   * value mapped to by the key
   * @throws IllegalArgumentException if the value mapped to by the key is not a
   * double
   * @throws IllegalArgumentException if the key is null
   */
  public double getDouble(String key, double defaultValue);
}
