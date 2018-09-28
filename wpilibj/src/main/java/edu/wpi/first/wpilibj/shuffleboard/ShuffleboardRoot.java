/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

/**
 * The root of the data placed in Shuffleboard. It contains the tabs, but no data is placed
 * directly in the root.
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

  /**
   * Updates all tabs.
   */
  void update();

  /**
   * Enables all widgets in Shuffleboard that offer user control over actuators.
   */
  void enableActuatorWidgets();

  /**
   * Disables all widgets in Shuffleboard that offer user control over actuators.
   */
  void disableActuatorWidgets();

}
