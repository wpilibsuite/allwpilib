// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

/**
 * The root of the data placed in Shuffleboard. It contains the tabs, but no data is placed directly
 * in the root.
 *
 * <p>This class is package-private to minimize API surface area.
 */
interface ShuffleboardRoot {
  /**
   * Gets the tab with the given title, creating it if it does not already exist.
   *
   * @param title the title of the tab
   * @return the tab with the given title
   */
  ShuffleboardTab getTab(String title);

  /** Updates all tabs. */
  void update();

  /** Enables all widgets in Shuffleboard that offer user control over actuators. */
  void enableActuatorWidgets();

  /** Disables all widgets in Shuffleboard that offer user control over actuators. */
  void disableActuatorWidgets();

  /**
   * Selects the tab in the dashboard with the given index in the range [0..n-1], where <i>n</i> is
   * the number of tabs in the dashboard at the time this method is called.
   *
   * @param index the index of the tab to select
   */
  void selectTab(int index);

  /**
   * Selects the tab in the dashboard with the given title.
   *
   * @param title the title of the tab to select
   */
  void selectTab(String title);
}
