// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;

/**
 * Persistent alert to be sent via NetworkTables. Alerts are tagged with a type of {@code ERROR},
 * {@code WARNING}, or {@code INFO} to denote urgency. See {@link
 * edu.wpi.first.wpilibj.Alert.AlertType AlertType} for suggested usage of each type. Alerts can be
 * displayed on supported dashboards, and are shown in a priority order based on type and recency of
 * activation.
 *
 * <p>Alerts should be created once and stored persistently, then updated to "active" or "inactive"
 * as necessary. {@link #set(boolean)} can be safely called periodically.
 *
 * <pre>
 * class MyClass {
 *   Alert alert = new Alert("Something went wrong", AlertType.WARNING);
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
 *   new Alert("Failed to load auto paths", AlertType.ERROR).set(true);
 * }
 * </pre>
 */
public class Alert {
  private static Map<String, SendableAlerts> groups = new HashMap<String, SendableAlerts>();

  private final AlertType m_type;
  private boolean m_active;
  private double m_activeStartTime;
  private String m_text;

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
  public Alert(String group, String text, AlertType type) {
    if (!groups.containsKey(group)) {
      groups.put(group, new SendableAlerts());
      SmartDashboard.putData(group, groups.get(group));
    }

    m_text = text;
    m_type = type;
    groups.get(group).m_alerts.add(this);
  }

  /**
   * Sets whether the alert should currently be displayed. When activated, the alert text will also
   * be sent to the console. This method can be safely called periodically.
   *
   * @param active Whether to display the alert.
   */
  public void set(boolean active) {
    if (active && !m_active) {
      m_activeStartTime = Timer.getFPGATimestamp();
    }
    m_active = active;
  }

  /**
   * Updates current alert text. Use this method to dynamically change the displayed alert, such as
   * including more details about the detected problem.
   *
   * @param text Text to be displayed when the alert is active.
   */
  public void setText(String text) {
    m_text = text;
  }

  private static final class SendableAlerts implements Sendable {
    private final Collection<Alert> m_alerts = new HashSet<>();

    private String[] getStrings(AlertType type) {
      return m_alerts.stream()
          .filter((Alert a) -> a.m_active && a.m_type == type)
          .sorted((Alert a, Alert b) -> Double.compare(b.m_activeStartTime, a.m_activeStartTime))
          .map((Alert a) -> a.m_text)
          .toArray(String[]::new);
    }

    @Override
    public void initSendable(SendableBuilder builder) {
      builder.setSmartDashboardType("Alerts");
      builder.addStringArrayProperty("errors", () -> getStrings(AlertType.ERROR), null);
      builder.addStringArrayProperty("warnings", () -> getStrings(AlertType.WARNING), null);
      builder.addStringArrayProperty("infos", () -> getStrings(AlertType.INFO), null);
    }
  }

  /** Represents an alert's level of urgency. */
  public enum AlertType {
    /**
     * High priority alert - displayed first on the dashboard with a red "X" symbol. Use this type
     * for problems which will seriously affect the robot's functionality and thus require immediate
     * attention.
     */
    ERROR,

    /**
     * Medium priority alert - displayed second on the dashboard with a yellow "!" symbol. Use this
     * type for problems which could affect the robot's functionality but do not necessarily require
     * immediate attention.
     */
    WARNING,

    /**
     * Low priority alert - displayed last on the dashboard with a green "i" symbol. Use this type
     * for problems which are unlikely to affect the robot's functionality, or any other alerts
     * which do not fall under ERROR or WARNING.
     */
    INFO
  }
}
