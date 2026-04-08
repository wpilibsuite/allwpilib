// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import org.wpilib.driverstation.Alert.Level;
import org.wpilib.hardware.hal.AlertJNI;
import org.wpilib.hardware.hal.simulation.AlertDataJNI;

/** Simulation for alerts. */
public final class AlertSim {
  private AlertSim() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /** Information about an alert. */
  public static class AlertInfo {
    AlertInfo(AlertDataJNI.AlertInfo info) {
      this.handle = info.handle;
      this.group = info.group;
      this.text = info.text;
      this.activeStartTime = info.activeStartTime;
      this.level =
          switch (info.level) {
            case AlertJNI.LEVEL_HIGH -> Level.HIGH;
            case AlertJNI.LEVEL_MEDIUM -> Level.MEDIUM;
            case AlertJNI.LEVEL_LOW -> Level.LOW;
            default -> throw new IllegalArgumentException("Unknown alert level: " + info.level);
          };
    }

    /** The handle of the alert. */
    @SuppressWarnings("MemberName")
    public final int handle;

    /** The group of the alert. */
    @SuppressWarnings("MemberName")
    public final String group;

    /** The text of the alert. */
    @SuppressWarnings("MemberName")
    public final String text;

    /** The time the alert became active. 0 if not active. */
    @SuppressWarnings("MemberName")
    public final long activeStartTime;

    /** The level of the alert (HIGH, MEDIUM, or LOW). */
    @SuppressWarnings("MemberName")
    public final Level level;

    /**
     * Returns whether the alert is currently active.
     *
     * @return true if the alert is active, false otherwise
     */
    public boolean isActive() {
      return activeStartTime != 0;
    }
  }

  /**
   * Gets the number of alerts. Note: this is not guaranteed to be consistent with the number of
   * alerts returned by GetAll.
   *
   * @return the number of alerts
   */
  public static int getCount() {
    return AlertDataJNI.getNumAlerts();
  }

  /**
   * Gets detailed information about each alert.
   *
   * @return Alerts
   */
  public static AlertInfo[] getAll() {
    AlertDataJNI.AlertInfo[] alertInfos = AlertDataJNI.getAlerts();
    AlertInfo[] infos = new AlertInfo[alertInfos.length];
    for (int i = 0; i < alertInfos.length; i++) {
      infos[i] = new AlertInfo(alertInfos[i]);
    }
    return infos;
  }

  /** Resets all alert simulation data. */
  public static void resetData() {
    AlertDataJNI.resetData();
  }
}
