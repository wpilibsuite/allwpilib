// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

/** JNI for alert data. */
public class AlertDataJNI {

  /**
   * High priority alert - displayed first with a red "X" symbol. Use this type for problems which
   * will seriously affect the robot's functionality and thus require immediate attention.
   */
  public static final int LEVEL_HIGH = 0;

  /**
   * Medium priority alert - displayed second with a yellow "!" symbol. Use this type for problems
   * which could affect the robot's functionality but do not necessarily require immediate
   * attention.
   */
  public static final int LEVEL_MEDIUM = 1;

  /**
   * Low priority alert - displayed last with a green "i" symbol. Use this type for problems which
   * are unlikely to affect the robot's functionality, or any other alerts which do not fall under
   * the other categories.
   */
  public static final int LEVEL_LOW = 2;

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

    public final int level; // LEVEL_HIGH, LEVEL_MEDIUM, LEVEL_LOW
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
