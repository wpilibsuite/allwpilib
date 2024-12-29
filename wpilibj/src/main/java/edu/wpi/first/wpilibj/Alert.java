// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;

/**
 * Persistent alert to be sent via NetworkTables. Alerts are tagged with a type of {@code kError},
 * {@code kWarning}, or {@code kInfo} to denote urgency. See {@link
 * edu.wpi.first.wpilibj.Alert.AlertType AlertType} for suggested usage of each type. Alerts can be
 * displayed on supported dashboards, and are shown in a priority order based on type and recency of
 * activation, with newly activated alerts first.
 *
 * <p>Alerts should be created once and stored persistently, then updated to "active" or "inactive"
 * as necessary. {@link #set(boolean)} can be safely called periodically.
 *
 * <p><b>This API is new for 2025, but is likely to change in future seasons to facilitate deeper
 * integration with the robot control system.</b>
 *
 * <pre>
 * class Robot {
 *   Alert alert = new Alert("Something went wrong", AlertType.kWarning);
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
 *   new Alert("Failed to load auto paths", AlertType.kError).set(true);
 * }
 * </pre>
 */
public class Alert implements AutoCloseable {
  /** Represents an alert's level of urgency. */
  public enum AlertType {
    /**
     * High priority alert - displayed first on the dashboard with a red "X" symbol. Use this type
     * for problems which will seriously affect the robot's functionality and thus require immediate
     * attention.
     */
    kError,

    /**
     * Medium priority alert - displayed second on the dashboard with a yellow "!" symbol. Use this
     * type for problems which could affect the robot's functionality but do not necessarily require
     * immediate attention.
     */
    kWarning,

    /**
     * Low priority alert - displayed last on the dashboard with a green "i" symbol. Use this type
     * for problems which are unlikely to affect the robot's functionality, or any other alerts
     * which do not fall under the other categories.
     */
    kInfo
  }

  private final AlertType m_type;
  private boolean m_active;
  private long m_activeStartTime;
  private String m_text;
  private Set<PublishedAlert> m_activeAlerts;

  /**
   * Creates a new alert in the default group - "Alerts". If this is the first to be instantiated,
   * the appropriate entries will be added to NetworkTables.
   *
   * @param text Text to be displayed when the alert is active.
   * @param type Alert urgency level.
   */
  public Alert(String text, AlertType type) {
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
  @SuppressWarnings("this-escape")
  public Alert(String group, String text, AlertType type) {
    m_type = type;
    m_text = text;
    m_activeAlerts = SendableAlerts.forGroup(group).getActiveAlertsStorage(type);
  }

  /**
   * Sets whether the alert should currently be displayed. This method can be safely called
   * periodically.
   *
   * @param active Whether to display the alert.
   */
  public void set(boolean active) {
    if (active == m_active) {
      return;
    }

    if (active) {
      m_activeStartTime = RobotController.getTime();
      m_activeAlerts.add(new PublishedAlert(m_activeStartTime, m_text));
    } else {
      m_activeAlerts.remove(new PublishedAlert(m_activeStartTime, m_text));
    }
    m_active = active;
  }

  /**
   * Gets whether the alert is active.
   *
   * @return whether the alert is active.
   */
  public boolean get() {
    return m_active;
  }

  /**
   * Updates current alert text. Use this method to dynamically change the displayed alert, such as
   * including more details about the detected problem.
   *
   * @param text Text to be displayed when the alert is active.
   */
  public void setText(String text) {
    if (text.equals(m_text)) {
      return;
    }
    var oldText = m_text;
    m_text = text;
    if (m_active) {
      m_activeAlerts.remove(new PublishedAlert(m_activeStartTime, oldText));
      m_activeAlerts.add(new PublishedAlert(m_activeStartTime, m_text));
    }
  }

  /**
   * Gets the current alert text.
   *
   * @return the current text.
   */
  public String getText() {
    return m_text;
  }

  /**
   * Get the type of this alert.
   *
   * @return the type
   */
  public AlertType getType() {
    return m_type;
  }

  @Override
  public void close() {
    set(false);
  }

  private record PublishedAlert(long timestamp, String text) implements Comparable<PublishedAlert> {
    private static final Comparator<PublishedAlert> comparator =
        Comparator.comparingLong((PublishedAlert alert) -> alert.timestamp())
            .reversed()
            .thenComparing(Comparator.comparing((PublishedAlert alert) -> alert.text()));

    @Override
    public int compareTo(PublishedAlert o) {
      return comparator.compare(this, o);
    }
  }

  private static final class SendableAlerts implements Sendable {
    private static final Map<String, SendableAlerts> groups = new HashMap<String, SendableAlerts>();

    private final Map<AlertType, Set<PublishedAlert>> m_alerts = new HashMap<>();

    /**
     * Returns a reference to the set of active alerts for the given type.
     *
     * @param type the type
     * @return reference to the set of active alerts for the type
     */
    public Set<PublishedAlert> getActiveAlertsStorage(AlertType type) {
      return m_alerts.computeIfAbsent(type, _type -> new TreeSet<>());
    }

    private String[] getStrings(AlertType type) {
      return getActiveAlertsStorage(type).stream().map(a -> a.text()).toArray(String[]::new);
    }

    @Override
    public void initSendable(SendableBuilder builder) {
      builder.setSmartDashboardType("Alerts");
      builder.addStringArrayProperty("errors", () -> getStrings(AlertType.kError), null);
      builder.addStringArrayProperty("warnings", () -> getStrings(AlertType.kWarning), null);
      builder.addStringArrayProperty("infos", () -> getStrings(AlertType.kInfo), null);
    }

    /**
     * Returns the SendableAlerts for a given group, initializing and publishing if it does not
     * already exist.
     *
     * @param group the group name
     * @return the SendableAlerts for the group
     */
    private static SendableAlerts forGroup(String group) {
      return groups.computeIfAbsent(
          group,
          _group -> {
            var sendable = new SendableAlerts();
            SmartDashboard.putData(_group, sendable);
            return sendable;
          });
    }
  }
}
