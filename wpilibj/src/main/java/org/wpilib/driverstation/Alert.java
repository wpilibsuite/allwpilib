// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import org.wpilib.hardware.hal.AlertJNI;

/**
 * Persistent alert to be sent via NetworkTables. Alerts are tagged with a type of {@code HIGH},
 * {@code MEDIUM}, or {@code LOW} to denote urgency. See {@link org.wpilib.driverstation.Alert.Level
 * Level} for suggested usage of each type. Alerts can be displayed on supported dashboards, and are
 * shown in a priority order based on type and recency of activation, with newly activated alerts
 * first.
 *
 * <p>Alerts should be created once and stored persistently, then updated to "active" or "inactive"
 * as necessary. {@link #set(boolean)} can be safely called periodically.
 *
 * <pre>
 * class Robot {
 *   Alert alert = new Alert("Something went wrong", Alert.Level.MEDIUM);
 *
 *   periodic() {
 *     alert.set(...);
 *   }
 * }
 * </pre>
 *
 * <p>Alternatively, alerts which are only used once at startup can be created and activated inline.
 *
 * <pre>
 * public Robot() {
 *   new Alert("Failed to load auto paths", Alert.Level.HIGH).set(true);
 * }
 * </pre>
 */
public class Alert implements AutoCloseable {
  /** Represents an alert's level of urgency. */
  public enum Level {
    /**
     * High priority alert - displayed first with a red "X" symbol. Use this type for problems which
     * will seriously affect the robot's functionality and thus require immediate attention.
     */
    HIGH(AlertJNI.LEVEL_HIGH),

    /**
     * Medium priority alert - displayed second with a yellow "!" symbol. Use this type for problems
     * which could affect the robot's functionality but do not necessarily require immediate
     * attention.
     */
    MEDIUM(AlertJNI.LEVEL_MEDIUM),

    /**
     * Low priority alert - displayed last with a green "i" symbol. Use this type for problems which
     * are unlikely to affect the robot's functionality, or any other alerts which do not fall under
     * the other categories.
     */
    LOW(AlertJNI.LEVEL_LOW);

    private final int m_value;

    Level(int value) {
      m_value = value;
    }
  }

  private final Level m_type;
  private int m_handle;

  /**
   * Creates a new alert in the default group - "Alerts". If this is the first to be instantiated,
   * the appropriate entries will be added to NetworkTables.
   *
   * @param text Text to be displayed when the alert is active.
   * @param type Alert urgency level.
   */
  public Alert(String text, Level type) {
    this("Alerts", text, type);
  }

  /**
   * Creates a new alert. If this is the first to be instantiated in its group, the appropriate
   * entries will be added to NetworkTables.
   *
   * @param group Group identifier, used as the entry name in NetworkTables.
   * @param text Text to be displayed when the alert is active.
   * @param type Alert urgency level.
   */
  public Alert(String group, String text, Level type) {
    m_type = type;
    m_handle = AlertJNI.createAlert(group, text, type.m_value);
  }

  /**
   * Sets whether the alert should currently be displayed. This method can be safely called
   * periodically.
   *
   * @param active Whether to display the alert.
   */
  public void set(boolean active) {
    AlertJNI.setAlertActive(m_handle, active);
  }

  /**
   * Gets whether the alert is active.
   *
   * @return whether the alert is active.
   */
  public boolean get() {
    return AlertJNI.isAlertActive(m_handle);
  }

  /**
   * Updates current alert text. Use this method to dynamically change the displayed alert, such as
   * including more details about the detected problem.
   *
   * @param text Text to be displayed when the alert is active.
   */
  public void setText(String text) {
    AlertJNI.setAlertText(m_handle, text);
  }

  /**
   * Gets the current alert text.
   *
   * @return the current text.
   */
  public String getText() {
    return AlertJNI.getAlertText(m_handle);
  }

  /**
   * Get the type of this alert.
   *
   * @return the type
   */
  public Level getType() {
    return m_type;
  }

  @Override
  public void close() {
    AlertJNI.destroyAlert(m_handle);
    m_handle = 0;
  }
}
