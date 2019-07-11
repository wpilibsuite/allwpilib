/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

/**
 * Represents the type of a widget in Shuffleboard. Using this is preferred over specifying raw
 * strings, to avoid typos and having to know or look up the exact string name for a desired widget.
 *
 * @see BuiltInWidgets the built-in widget types
 */
public interface WidgetType {
  /**
   * Gets the string type of the widget as defined by that widget in Shuffleboard.
   */
  String getWidgetName();
}
