// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

import java.io.IOException;

/** JNI for alert data. */
public class AlertDataJNI {
  /** Information about an alert. */
  public static class AlertInfo {
    public AlertInfo(String group, String id, String text, long activeStartTime, int level) {
      this.group = group;
      this.id = id;
      this.text = text;
      this.activeStartTime = activeStartTime;
      this.level = level;
    }

    public final String group;

    public final String id;

    public final String text;

    public final long activeStartTime; // 0 if not active

    public final int level; // ALERT_LEVEL_HIGH, ALERT_LEVEL_MEDIUM, ALERT_LEVEL_LOW
  }

  static {
    try {
      WPIUtilJNI.forceLoad();
    } catch (IOException ex) {
      ex.printStackTrace();
      System.exit(1);
    }
  }

  /**
   * Gets the number of alerts. Note: this is not guaranteed to be consistent with the number of
   * alerts returned by getAlerts, so the latter's return value should be used to determine how many
   * alerts were actually filled in.
   *
   * @return the number of alerts
   */
  public static native int getNumAlerts();

  /**
   * Gets detailed information about each alert.
   *
   * @return Array of information about each alert
   */
  public static native AlertInfo[] getAlerts();

  /** Resets all alert simulation data. */
  public static native void resetData();

  /** Utility class. */
  private AlertDataJNI() {}
}
