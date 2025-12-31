// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

/**
 * Interface for individual tunable entries. Intended for use by backends; most users should use
 * the TunableTable class.
 */
public interface TunableEntry {
  /**
   * Sets property for a value. Properties are stored as a key/value map.
   *
   * @param key property key
   * @param value property value
   */
  void setProperty(String key, String value);

  /**
   * Updates an int.
   *
   * @param curValue current value
   * @return updated value
   */
  default int updateInt(int curValue) {
    return (int) updateLong(curValue);
  }

  /**
   * Gets an int.
   *
   * @param defaultValue the default value if no value has been set
   */
  default int getInt(int defaultValue) {
    return (int) getLong(defaultValue);
  }

  /**
   * Sets an int.
   *
   * @param value the value
   */
  default void setInt(int value) {
    setLong(value);
  }

  /**
   * Updates a long.
   *
   * @param curValue current value
   * @return updated value
   */
  long updateLong(long curValue);

  /**
   * Gets a long.
   *
   * @param defaultValue the default value if no value has been set
   */
  long getLong(long defaultValue);

  /**
   * Sets a long.
   *
   * @param value the value
   */
  void setLong(long value);

  /**
   * Updates a double.
   *
   * @param curValue current value
   * @return updated value
   */
  double updateDouble(double curValue);

  /**
   * Gets a double.
   *
   * @param defaultValue the default value if no value has been set
   */
  double getDouble(double defaultValue);

  /**
   * Sets a double.
   *
   * @param value the value
   */
  void setDouble(double value);
}
