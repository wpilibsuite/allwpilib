// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal.simulation;

import org.wpilib.hardware.hal.JNIWrapper;

/** JNI for alert data. */
public class AlertDataJNI extends JNIWrapper {
  /** Information about an alert. */
  public static class AlertInfo {
    public AlertInfo(int handle, String group, String text, long activeStartTime, int level) {
      this.handle = handle;
      this.group = group;
      this.text = text;
      this.activeStartTime = activeStartTime;
      this.level = level;
    }

    public final int handle;

    public final String group;

    public final String text;

    public final long activeStartTime; // 0 if not active

    public final int level; // ALERT_LEVEL_HIGH, ALERT_LEVEL_MEDIUM, ALERT_LEVEL_LOW
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
