// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/**
 * The AlertJNI class directly wraps the C++ HAL Alert.
 *
 * <p>This class is not meant for direct use by teams. Instead, the org.wpilib.driverstation.Alert
 * class, which corresponds to the C++ Alert class, should be used.
 *
 * @see "wpi/hal/Alert.h"
 */
public class AlertJNI extends JNIWrapper {
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

  /**
   * Creates an alert.
   *
   * @param group Group identifier
   * @param text Text to be displayed when the alert is active
   * @param level Alert urgency level (LEVEL_HIGH, LEVEL_MEDIUM, LEVEL_LOW)
   * @return the created alert handle
   * @see "HAL_CreateAlert"
   */
  public static native int createAlert(String group, String text, int level);

  /**
   * Destroys an alert.
   *
   * @param alertHandle the alert handle
   * @see "HAL_DestroyAlert"
   */
  public static native void destroyAlert(int alertHandle);

  /**
   * Sets whether the alert should be displayed. This method can be safely be called periodically.
   *
   * @param alertHandle the alert handle
   * @param active true to display the alert, false to hide it
   * @see "HAL_SetAlertActive"
   */
  public static native void setAlertActive(int alertHandle, boolean active);

  /**
   * Checks if an alert is active.
   *
   * @param alertHandle the alert handle
   * @return true if the alert is active
   * @see "HAL_IsAlertActive"
   */
  public static native boolean isAlertActive(int alertHandle);

  /**
   * Sets the text of the alert. Use this method to dynamically change the displayed alert, such as
   * including more details about the detected problem.
   *
   * @param alertHandle Alert handle.
   * @param text new text to be displayed when the alert is active
   * @see "HAL_SetAlertText"
   */
  public static native void setAlertText(int alertHandle, String text);

  /**
   * Gets the text of the alert.
   *
   * @param alertHandle Alert handle.
   * @return the text displayed when the alert is active
   * @see "HAL_GetAlertText"
   */
  public static native String getAlertText(int alertHandle);

  /** Utility class. */
  private AlertJNI() {}
}
