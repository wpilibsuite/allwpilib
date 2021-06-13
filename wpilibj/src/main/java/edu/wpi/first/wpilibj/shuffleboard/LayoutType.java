// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
   *
   * @return The string type of the layout.
   */
  String getLayoutName();
}
