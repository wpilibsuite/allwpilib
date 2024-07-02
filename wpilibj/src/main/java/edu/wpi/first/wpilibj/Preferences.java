// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.networktables.MultiSubscriber;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableEvent;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.NetworkTableListener;
import edu.wpi.first.networktables.StringPublisher;
import edu.wpi.first.networktables.StringTopic;
import edu.wpi.first.networktables.Topic;
import java.util.Collection;
import java.util.EnumSet;

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
  private static final String kTableName = "Preferences";

  private static final String kSmartDashboardType = "RobotPreferences";

  /** The network table. */
  private static NetworkTable m_table;

  private static StringPublisher m_typePublisher;
  private static MultiSubscriber m_tableSubscriber;
  private static NetworkTableListener m_listener;

  /** Creates a preference class. */
  private Preferences() {}

  static {
    setNetworkTableInstance(NetworkTableInstance.getDefault());
    HAL.report(tResourceType.kResourceType_Preferences, 0);
  }

  /**
   * Set the NetworkTable instance used for entries. For testing purposes; use with caution.
   *
   * @param inst NetworkTable instance
   */
  public static synchronized void setNetworkTableInstance(NetworkTableInstance inst) {
    m_table = inst.getTable(kTableName);
    if (m_typePublisher != null) {
      m_typePublisher.close();
    }
    m_typePublisher =
        m_table
            .getStringTopic(".type")
            .publishEx(
                StringTopic.kTypeString, "{\"SmartDashboard\":\"" + kSmartDashboardType + "\"}");
    m_typePublisher.set(kSmartDashboardType);

    // Subscribe to all Preferences; this ensures we get the latest values
    // ahead of a getter call.
    if (m_tableSubscriber != null) {
      m_tableSubscriber.close();
    }
    m_tableSubscriber = new MultiSubscriber(inst, new String[] {m_table.getPath() + "/"});

    // Listener to set all Preferences values to persistent
    // (for backwards compatibility with old dashboards).
    if (m_listener != null) {
      m_listener.close();
    }
    m_listener =
        NetworkTableListener.createListener(
            m_tableSubscriber,
            EnumSet.of(NetworkTableEvent.Kind.kImmediate, NetworkTableEvent.Kind.kPublish),
            event -> {
              if (event.topicInfo != null) {
                Topic topic = event.topicInfo.getTopic();
                if (!topic.equals(m_typePublisher.getTopic())) {
                  event.topicInfo.getTopic().setPersistent(true);
                }
              }
            });
  }

  /**
   * Gets the preferences keys.
   *
   * @return a collection of the keys
   */
  public static Collection<String> getKeys() {
    return m_table.getKeys();
  }

  /**
   * Puts the given string into the preferences table.
   *
   * @param key the key
   * @param value the value
   * @throws NullPointerException if value is null
   */
  public static void setString(String key, String value) {
    requireNonNullParam(value, "value", "setString");

    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setString(value);
    entry.setPersistent();
  }

  /**
   * Puts the given string into the preferences table if it doesn't already exist.
   *
   * @param key The key
   * @param value The value
   */
  public static void initString(String key, String value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDefaultString(value);
    entry.setPersistent();
  }

  /**
   * Puts the given int into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public static void setInt(String key, int value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDouble(value);
    entry.setPersistent();
  }

  /**
   * Puts the given int into the preferences table if it doesn't already exist.
   *
   * @param key The key
   * @param value The value
   */
  public static void initInt(String key, int value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDefaultDouble(value);
    entry.setPersistent();
  }

  /**
   * Puts the given double into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public static void setDouble(String key, double value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDouble(value);
    entry.setPersistent();
  }

  /**
   * Puts the given double into the preferences table if it doesn't already exist.
   *
   * @param key The key
   * @param value The value
   */
  public static void initDouble(String key, double value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDefaultDouble(value);
    entry.setPersistent();
  }

  /**
   * Puts the given float into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public static void setFloat(String key, float value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDouble(value);
    entry.setPersistent();
  }

  /**
   * Puts the given float into the preferences table if it doesn't already exist.
   *
   * @param key The key
   * @param value The value
   */
  public static void initFloat(String key, float value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDefaultDouble(value);
    entry.setPersistent();
  }

  /**
   * Puts the given boolean into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public static void setBoolean(String key, boolean value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setBoolean(value);
    entry.setPersistent();
  }

  /**
   * Puts the given boolean into the preferences table if it doesn't already exist.
   *
   * @param key The key
   * @param value The value
   */
  public static void initBoolean(String key, boolean value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDefaultBoolean(value);
    entry.setPersistent();
  }

  /**
   * Puts the given long into the preferences table.
   *
   * @param key the key
   * @param value the value
   */
  public static void setLong(String key, long value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setInteger(value);
    entry.setPersistent();
  }

  /**
   * Puts the given long into the preferences table if it doesn't already exist.
   *
   * @param key The key
   * @param value The value
   */
  public static void initLong(String key, long value) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.setDefaultInteger(value);
    entry.setPersistent();
  }

  /**
   * Returns whether there is a key with the given name.
   *
   * @param key the key
   * @return if there is a value at the given key
   */
  public static boolean containsKey(String key) {
    return m_table.containsKey(key);
  }

  /**
   * Remove a preference.
   *
   * @param key the key
   */
  public static void remove(String key) {
    NetworkTableEntry entry = m_table.getEntry(key);
    entry.clearPersistent();
    entry.unpublish();
  }

  /** Remove all preferences. */
  public static void removeAll() {
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
  public static String getString(String key, String backup) {
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
  public static int getInt(String key, int backup) {
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
  public static double getDouble(String key, double backup) {
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
  public static boolean getBoolean(String key, boolean backup) {
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
  public static float getFloat(String key, float backup) {
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
  public static long getLong(String key, long backup) {
    return m_table.getEntry(key).getInteger(backup);
  }
}
