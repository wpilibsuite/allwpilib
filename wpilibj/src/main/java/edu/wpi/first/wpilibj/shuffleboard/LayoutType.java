/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

/**
 * Represents the type of a layout in Shuffleboard. Using this is preferred over specifying raw
 * strings, to avoid typos and having to know or look up the exact string name for a desired layout.
 *
 * @see BuiltInWidgets the built-in widget types
 */
public interface LayoutType {
  /**
   * Gets the string type of the layout as defined by that layout in Shuffleboard.
   */
  String getLayoutName();
}
